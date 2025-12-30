use crate::config;
use crate::config::{OutputKind, KnownOutputKind};
use crate::execute;
use glob::glob;
use globset::{Glob, GlobSetBuilder};
use std::collections::HashMap;
use std::io;
use std::fs;
use std::env;
use std::path::{Path, PathBuf};
use std::process::Command;
use crate::cache::cache;
use crate::context::Context;

fn copy_dir_all(src: &Path, dst: &Path) -> io::Result<()> {
    if !dst.exists() {
        fs::create_dir_all(dst)?;
    }

    for entry in fs::read_dir(src)? {
        let entry = entry?;
        let ty = entry.file_type()?;
        let dest_path = dst.join(entry.file_name());

        if ty.is_dir() {
            copy_dir_all(&entry.path(), &dest_path)?;
        } else if ty.is_file() {
            fs::copy(entry.path(), dest_path)?;
        } else if ty.is_symlink() {
            handle_symlink(&entry.path(), &dest_path)?;
        }
    }
    Ok(())
}

fn handle_symlink(src: &Path, dst: &Path) -> io::Result<()> {
    #[cfg(unix)]
    {
        let target = fs::read_link(src)?;
        std::os::unix::fs::symlink(target, dst)
    }

    #[cfg(windows)]
    {
        let target = fs::read_link(src)?;
        if target.is_dir() {
            std::os::windows::fs::symlink_dir(target, dst)
        } else {
            std::os::windows::fs::symlink_file(target, dst)
        }
    }

    #[cfg(not(any(unix, windows)))]
    {
        Err(io::Error::new(io::ErrorKind::Other, "Symlinks not supported"))
    }
}

pub fn execute_target(
    name: &str,
    config: &config::Config,
    toolchains: &HashMap<String, crate::tools::tools::Toolchain>,
    formats: &HashMap<String, crate::tools::tools::Format>,
    executed: &mut HashMap<String, Vec<PathBuf>>,
    build_dir: &Path,
    cache: &cache::CacheBuffer,
    context: &Context,
) -> Result<Vec<PathBuf>, std::io::Error> {
    if let Some(existing_outputs) = executed.get(name) {
        return Ok(existing_outputs.clone());
    }

    let mut outputs = Vec::new();

    if let Some(target) = config.targets.get(name) {
        let mut inputs = Vec::new();

        for dep in &target.before {
            let result = execute_target(dep, config, toolchains, formats, executed, build_dir, cache, context);
            if let Err(e) = result {
                return Err(io::Error::new(
                    io::ErrorKind::Other,
                    format!("Target {} failed: {:?}", dep, e),
                ));
            }
        }
        
        for dep in &target.depends {
            let dep_outputs = execute_target(dep, config, toolchains, formats, executed, build_dir, cache, context);
            if let Err(e) = dep_outputs {
                return Err(io::Error::new(
                    io::ErrorKind::Other,
                    format!("Target {} failed: {:?}", dep, e),
                ));
            }
            inputs.extend(dep_outputs?);
        }

        if let Some(message) = &target.message {
            println!("{}", message);
        }

        let target_path = PathBuf::from(&target.path);

        if let Some(files_sov) = &target.files {
            let files = files_sov.into_vec();

            let mut builder = GlobSetBuilder::new();
            for pat in files {
                let pattern = format!("{}/**/{}", target_path.display(), pat);
                builder.add(Glob::new(&pattern).map_err(|e| io::Error::new(io::ErrorKind::Other, e))?);
            }
            let glob_set = builder.build().map_err(|e| io::Error::new(io::ErrorKind::Other, e))?;

            let search_pattern = format!("{}/**/*", target_path.display());
            for entry in glob(&search_pattern).map_err(|e| io::Error::new(io::ErrorKind::Other, e))? {
                if let Ok(full_path) = entry {
                    if glob_set.is_match(&full_path) {
                        inputs.push(full_path);
                    }
                }
            }
        }

        let mut for_each = match &target.out {
            OutputKind::Known(kind) => match kind {
                KnownOutputKind::Object => true,
                KnownOutputKind::Executable => false,
                KnownOutputKind::StaticLibrary => false,
                _ => true,
            },
            OutputKind::Custom(_) => true,
        };
        if let Some(for_each_explicit) = &target.for_each {
            for_each = *for_each_explicit;
        }

        if !inputs.is_empty() {
            if for_each {
                for input in &inputs {
                    let mut replacements = HashMap::new();
                    let rel = input.strip_prefix(&target_path).unwrap_or(input);
                    replacements.insert("input".to_string(), rel.to_string_lossy().to_string());
                    let rel_output_path = crate::execute::replace_placeholders(&target.out_path, &replacements)?;
                    let mut output_path = build_dir.join(rel_output_path);

                    let temp_inputs: Vec<&Path> = vec![input];

                    let result = execute::execute(temp_inputs, &output_path, &target.out, config, toolchains, formats, cache, &context.mode, &config.tools.default_toolchain, context.rebuild);
                    if result.is_err() {
                        return Err(io::Error::new(
                            io::ErrorKind::Other,
                            format!("Execution of {} failed: {}", input.display(), result.err().unwrap()),
                        ));
                    }

                    outputs.push(output_path);
                }
            } else {
                let mut replacements = HashMap::new();
                let input_path = &inputs[0];
                let rel = input_path.strip_prefix(&target_path).unwrap_or(input_path);
                replacements.insert("input".to_string(), rel.to_string_lossy().to_string());
                let rel_output_path = crate::execute::replace_placeholders(&target.out_path, &replacements)?;
                let mut output_path = build_dir.join(rel_output_path);

                // TODO: not always .exe
                if matches!(target.out, OutputKind::Known(KnownOutputKind::Executable)) {
                    #[cfg(windows)]
                    {
                        let mut file_name = output_path.file_name().ok_or_else(|| {
                            io::Error::new(io::ErrorKind::Other, "Couldn't get filename of output")
                        })?.to_os_string();
                        file_name.push(".exe");
                        output_path.set_file_name(file_name);
                    }
                }

                if let Some(parent) = output_path.parent() {
                    fs::create_dir_all(parent)?;
                }

                let temp_inputs: Vec<&Path> = inputs.iter().map(|p| p.as_path()).collect();
                let result = execute::execute(temp_inputs, &output_path, &target.out, config, toolchains, formats, cache, &context.mode, &config.tools.default_toolchain, context.rebuild);
                if result.is_err() {
                    return Err(io::Error::new(
                        io::ErrorKind::Other,
                        format!("Execution of {} failed", output_path.display()),
                    ));
                }

                outputs.push(output_path);
            }
        }

        if let Some(command) = &target.run {
            let mut replacements = HashMap::new();
            replacements.insert("build_dir".to_string(), build_dir.to_string_lossy().to_string());

            let cmd = crate::execute::replace_placeholders(command, &replacements)?;
            // run
            if let Some(parts) = shlex::split(&cmd) {
                if let Some(command) = parts.first() {
                    let result = match command.as_str() {
                        "execute" => {
                            if let Some(program) = parts.get(1) {
                                let args = &parts[2..];
                                Command::new(program).args(args).status().map(|status| status.success()).unwrap_or(false)
                            } else {
                                false
                            }
                        }
                        "delete" => {
                            if let Some(target) = parts.get(1) {
                                let path = Path::new(target);

                                if !path.exists() {
                                    true
                                } else if path.is_file() {
                                    fs::remove_file(target).is_ok()
                                } else if path.is_dir() {
                                    fs::remove_dir_all(target).is_ok()
                                } else {
                                    false
                                }
                            } else {
                                false
                            }
                        }
                        "copy" => {
                            if let (Some(src), Some(dst)) = (parts.get(1), parts.get(2)) {
                                let src_path = Path::new(src);
                                let dst_path = Path::new(dst);

                                if src_path.is_file() {
                                    fs::copy(src_path, dst_path).is_ok()
                                } else if src_path.is_dir() {
                                    copy_dir_all(src_path, dst_path).is_ok()
                                } else {
                                    false
                                }
                            } else {
                                false
                            }
                        }
                        "move" => {
                            if let (Some(src), Some(dst)) = (parts.get(1), parts.get(2)) {
                                fs::rename(src, dst).is_ok()
                            } else {
                                false
                            }
                        }

                        _ => false
                    };

                    if !result {
                        return Err(io::Error::new(
                            io::ErrorKind::Other,
                            format!("Execution of command '{}' failed", command),
                        ));
                    }
                }
            }
        }

        executed.insert(name.to_string(), outputs.clone());
    } else {
        println!("Skipping unknown target: {}", name);
    }

    Ok(outputs)
}