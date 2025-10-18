use std::collections::HashMap;
use std::io::BufRead;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::{fs, io};

use crate::cache::cache;
use crate::util::{get_format, get_toolchain};

fn replace_placeholders(s: &str, replacements: &HashMap<String, String>) -> Result<String, std::io::Error> {
    let mut result = String::new();
    let mut chars = s.chars().peekable();

    while let Some(c) = chars.next() {
        if c == '$' && chars.peek() == Some(&'{') {
            chars.next();
            let mut placeholder = String::new();

            while let Some(&ch) = chars.peek() {
                chars.next();
                if ch == '}' { break; }
                placeholder.push(ch);
            }

            let key = placeholder.trim().to_lowercase();
            let replacement = replacements.get(&key).ok_or_else(|| {
                std::io::Error::new(std::io::ErrorKind::InvalidInput, format!("Unknown placeholder: {}", key))
            })?;
            result.push_str(replacement);
        } else {
            result.push(c);
        }
    }

    Ok(result)
}

pub fn execute(
    inputs: Vec<&Path>,
    output: &Path,
    out: &crate::config::OutputKind,
    config: &crate::config::Config,
    toolchains: &HashMap<String, crate::tools::tools::Toolchain>,
    formats: &HashMap<String, crate::tools::tools::Format>,
    cache: &cache::CacheBuffer,
    mut force_rebuild: bool,
) -> Result<bool, std::io::Error> {
    if !output.exists() {
        force_rebuild = true;
    }

    let toolchain = get_toolchain(&config.tools.default, toolchains)?;

    let input_strs: Vec<String> = inputs.iter()
        .map(|p| p.display().to_string())
        .collect();


    'tools: for (_tool_name, tool) in &toolchain.tools {
        if tool.when.out != *out {
            continue;
        }

        //TODO: temporary solution
        if let Some(exts) = &tool.when.ext {
            for input in &inputs {
                let ext = input.extension()
                    .and_then(|e| e.to_str())
                    .unwrap_or("");

                if !exts.iter().any(|v| v == ext) {
                    continue 'tools;
                }
            }
        }

        // Found a tool
        let mut replacements = HashMap::new();
        replacements.insert("input".to_string(), input_strs.join(" "));
        replacements.insert("output".to_string(), output.to_string_lossy().to_string());

        let mut flags = Vec::new();
        if let Some(flags_name) = &tool.flags {
            let flags_set = toolchain.flags.get(flags_name).ok_or_else(|| {
                io::Error::new(
                    io::ErrorKind::NotFound,
                    format!("Flags '{}' not found", flags_name)
                )
            })?;

            flags.extend(flags_set.default.clone());
            let os_flags = match std::env::consts::OS {
                "windows" => &flags_set.windows,
                "linux" => &flags_set.linux,
                "macos" => &flags_set.macos,
                _ => &Vec::new(),
            };
            flags.extend(os_flags.clone());
        }
        replacements.insert("flags".to_string(), flags.join(" "));

        let mut cache_inputs: Vec<PathBuf> = inputs.iter().map(|p| p.to_path_buf()).collect();
        if let Some(dep_file_raw) = &tool.deps {
            let dep_file = replace_placeholders(&dep_file_raw, &replacements)?;
            if Path::new(&dep_file).exists() {
                let file = fs::File::open(&dep_file)?;
                let reader = io::BufReader::new(file);
                let mut lines: Vec<String> = reader.lines().collect::<Result<_, _>>()?;

                let format_name = tool.format.clone().ok_or_else(|| {
                    io::Error::new(io::ErrorKind::Other,format!("No format defined"))
                })?;
                let format = get_format(&format_name, formats)?;

                let start_skip = format.start.as_ref().map(|r| r.ignore_lines as usize).unwrap_or(0);
                let end_skip   = format.end  .as_ref().map(|r| r.ignore_lines as usize).unwrap_or(0);

                if lines.len() >= start_skip + end_skip {
                    let range_end = lines.len() - end_skip;
                    let mut deps = Vec::new();

                    for mut line in lines.drain(start_skip..range_end) /* FIXME: check */ {
                        line = line.trim().to_string();

                        if let Some(start) = &format.start {
                            if let Some(trim) = &start.trim {
                                if line.starts_with(&trim.condition) {
                                    let n = trim.remove as usize;
                                    if line.len() >= n {
                                        line.drain(0..n);
                                    } else {
                                        line.clear();
                                    }
                                }
                            }
                        }

                        if let Some(end) = &format.end {
                            if let Some(trim) = &end.trim {
                                if line.ends_with(&trim.condition) {
                                    let n = trim.remove as usize;
                                    if line.len() >= n {
                                        let new_len = line.len() - n;
                                        line.truncate(new_len);
                                    } else {
                                        line.clear();
                                    }
                                }
                            }
                        }

                        let s = line.trim();
                        if !s.is_empty() {
                            if let Some(split) = &format.split {
                                for part in s.split(split) {
                                    deps.push(part.to_string());
                                }
                            } else {
                                deps.push(s.to_string());
                            }
                        }
                    }

                    for dep_str in &deps {
                        let dep = PathBuf::from(dep_str);
                        if !dep.exists() {
                            eprintln!("Warning: Dependency '{}' does not exist.", dep.display());
                        } else {
                            cache_inputs.push(dep);
                        }
                    }
                }
            }
        }

        if !force_rebuild && cache::check_built(&cache_inputs, &output.to_string_lossy(), cache) {
            return Ok(false);
        }

        if let Some(parent) = output.parent() {
            fs::create_dir_all(parent)?;
        }

        let commands: Vec<String> = tool.command.iter()
            .map(|cmd| replace_placeholders(cmd, &replacements))
            .collect::<Result<Vec<String>, std::io::Error>>()?;

        if let Some(raw_message) = &tool.message {
            let message = replace_placeholders(raw_message, &replacements)?;
            println!("{}", message);
        }

        for command in &commands {
            let mut parts = command.split_whitespace();
            if let Some(program) = parts.next() {
                let args: Vec<&str> = parts.collect();

                #[cfg(target_os = "windows")]
                let status = {
                    let mut cmdline = vec![program.to_string()];
                    cmdline.extend(args.iter().map(|s| s.to_string()));

                    let cmdline = cmdline
                        .into_iter()
                        .map(|s| {
                            if s.contains(' ') { format!("\"{}\"", s) } else { s }
                        })
                        .collect::<Vec<_>>()
                        .join(" ");

                    Command::new("cmd").args(&["/C", &cmdline]).status()?
                };

                #[cfg(not(target_os = "windows"))]
                let status = {
                    let mut cmdline = vec![program.to_string()];
                    cmdline.extend(args.iter().map(|s| s.to_string()));

                    let cmdline = cmdline
                        .into_iter()
                        .map(|s| {
                            if s.contains(' ') { format!("'{}'", s) } else { s }
                        })
                        .collect::<Vec<_>>()
                        .join(" ");

                    Command::new("sh").args(&["-c", &cmdline]).status()?
                };

                if !status.success() {
                    return Err(io::Error::new(
                        io::ErrorKind::Other,
                        format!("Compilation failed for output: {}", output.display()),
                    ));
                }
            }
        }

        cache_inputs = inputs.iter().map(|p| p.to_path_buf()).collect();
        if let Some(dep_file_raw) = &tool.deps {
            let dep_file = replace_placeholders(&dep_file_raw, &replacements)?;
            if Path::new(&dep_file).exists() {
                let file = fs::File::open(&dep_file)?;
                let reader = io::BufReader::new(file);
                let mut lines: Vec<String> = reader.lines().collect::<Result<_, _>>()?;

                let format_name = tool.format.clone().ok_or_else(|| {
                    io::Error::new(io::ErrorKind::Other,format!("No format defined"))
                })?;
                let format = get_format(&format_name, formats)?;

                let start_skip = format.start.as_ref().map(|r| r.ignore_lines as usize).unwrap_or(0);
                let end_skip   = format.end  .as_ref().map(|r| r.ignore_lines as usize).unwrap_or(0);

                if lines.len() >= start_skip + end_skip {
                    let range_end = lines.len() - end_skip;
                    let mut deps = Vec::new();

                    for mut line in lines.drain(start_skip..range_end) /* FIXME: check */ {
                        line = line.trim().to_string();

                        if let Some(start) = &format.start {
                            if let Some(trim) = &start.trim {
                                if line.starts_with(&trim.condition) {
                                    let n = trim.remove as usize;
                                    if line.len() >= n {
                                        line.drain(0..n);
                                    } else {
                                        line.clear();
                                    }
                                }
                            }
                        }

                        if let Some(end) = &format.end {
                            if let Some(trim) = &end.trim {
                                if line.ends_with(&trim.condition) {
                                    let n = trim.remove as usize;
                                    if line.len() >= n {
                                        let new_len = line.len() - n;
                                        line.truncate(new_len);
                                    } else {
                                        line.clear();
                                    }
                                }
                            }
                        }

                        let s = line.trim();
                        if !s.is_empty() {
                            if let Some(split) = &format.split {
                                for part in s.split(split) {
                                    deps.push(part.to_string());
                                }
                            } else {
                                deps.push(s.to_string());
                            }
                        }
                    }

                    for dep_str in &deps {
                        let dep = PathBuf::from(dep_str);
                        if !dep.exists() {
                            eprintln!("Warning: Dependency '{}' does not exist.", dep.display());
                        } else {
                            cache_inputs.push(dep);
                        }
                    }
                }
            }
        }

        cache::write_built(&cache_inputs, &output.to_string_lossy(), cache);
        return Ok(true);
    }

    return Err(io::Error::new(
        io::ErrorKind::Other,
        format!("Didn't find any fitting tool"),
    ));
}
