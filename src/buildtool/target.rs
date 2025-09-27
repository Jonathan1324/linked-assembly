use crate::config;
use crate::config::OutputKind;
use crate::execute;
use glob::glob;
use std::collections::HashMap;
use std::io;
use std::process::Command;
use std::fs;
use std::env;
use std::path::{Path, PathBuf};
use crate::cache::cache;

pub fn execute_target(
    name: &str,
    config: &config::Config,
    toolchains: &HashMap<String, crate::tools::tools::Toolchain>,
    formats: &HashMap<String, crate::tools::tools::Format>,
    executed: &mut HashMap<String, Vec<PathBuf>>,
    build_dir: &Path,
    cache: &cache::CacheBuffer,
) -> Result<Vec<PathBuf>, std::io::Error> {
    if let Some(existing_outputs) = executed.get(name) {
        return Ok(existing_outputs.clone());
    }

    let mut outputs = Vec::new();

    if let Some(target) = config.targets.get(name) {
        let mut inputs = Vec::new();

        for dep in &target.before {
            let result = execute_target(dep, config, toolchains, formats, executed, build_dir, cache);
            if result.is_err() {
                return Err(io::Error::new(
                    io::ErrorKind::Other,
                    format!("Target {} failed", dep),
                ));
            }
        }
        
        for dep in &target.depends {
            let dep_outputs = execute_target(dep, config, toolchains, formats, executed, build_dir, cache);
            if dep_outputs.is_err() {
                return Err(io::Error::new(
                    io::ErrorKind::Other,
                    format!("Target {} failed", dep),
                ));
            }
            inputs.extend(dep_outputs.unwrap());
        }

        if let Some(message) = &target.message {
            println!("{}", message);
        }

        let target_path = if Path::new(&target.path).is_absolute() {
            PathBuf::from(&target.path)
        } else {
            env::current_dir().unwrap().join(&target.path)
        };

        if let Some(files) = &target.files {
            let glob_pattern = format!("{}/**/{}", target_path.display(), files);
            for entry in glob(&glob_pattern).unwrap() {
                if let Ok(full_path) = entry {
                    inputs.push(full_path);
                }
            }
        }

        let mut for_each = match target.out {
            OutputKind::Object => { true }
            OutputKind::Executable => { false }
            OutputKind::StaticLibrary => { false }

            _ => { true }
        };
        if let Some(for_each_explicit) = &target.for_each {
            for_each = *for_each_explicit;
        }

        if !inputs.is_empty() {
            if for_each {
                for input in &inputs {
                    let mut output_path = build_dir.join(input.strip_prefix(env::current_dir().unwrap()).unwrap());
                    let file_name = if let Some(name) = &target.name {
                        name.clone().into()
                    } else {
                        match target.out {
                            config::OutputKind::Object => {
                                let mut name = output_path.file_name().unwrap().to_os_string();
                                name.push(".o");
                                name
                            }
                            _ => {
                                Path::new(output_path.file_name().unwrap()).file_stem().unwrap().to_os_string()
                            }
                        }
                    };
                    output_path.set_file_name(file_name);

                    let temp_inputs: Vec<&Path> = vec![input];

                    let result = execute::execute(temp_inputs, &output_path, &target.out, config, toolchains, formats, cache, false); //TODO
                    if result.is_err() {
                        return Err(io::Error::new(
                            io::ErrorKind::Other,
                            format!("Execution of {} failed", input.display()),
                        ));
                    }

                    outputs.push(output_path);
                }
            } else {
                let mut output_path = build_dir.join(inputs[0].strip_prefix(env::current_dir().unwrap()).unwrap());
                let mut file_name = if let Some(name) = &target.name {
                    output_path = build_dir.to_path_buf().join("placeholder"); // TODO: FIXME: WHATEVER: REALLY ONLY TEMPORARY; VERY UGLY
                    name.clone().into()
                } else {
                    match target.out {
                        config::OutputKind::Object => {
                            let mut name = output_path.file_name().unwrap().to_os_string();
                            name.push(".o");
                            name
                        }
                        _ => {
                            Path::new(output_path.file_name().unwrap()).file_stem().unwrap().to_os_string()
                        }
                    }
                };
                if matches!(target.out, config::OutputKind::Executable) {
                    #[cfg(windows)]
                    {
                        file_name.push(".exe");
                    }
                }
                output_path.set_file_name(file_name);

                if let Some(parent) = output_path.parent() {
                    fs::create_dir_all(parent).unwrap();
                }

                let temp_inputs: Vec<&Path> = inputs.iter().map(|p| p.as_path()).collect();
                let result = execute::execute(temp_inputs, &output_path, &target.out, config, toolchains, formats, cache, false); //TODO
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
            // run
            let mut parts = command.split_whitespace();
            if let Some(program) = parts.next() {
                let args: Vec<&str> = parts.collect();

                let status = Command::new(program)
                    .args(&args)
                    .status();

                // TODO: Check status
            }
        }

        executed.insert(name.to_string(), outputs.clone());
    } else {
        println!("Skipping unknown target: {}", name);
    }

    Ok(outputs)
}