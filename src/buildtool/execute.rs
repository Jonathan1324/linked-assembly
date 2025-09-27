use std::collections::HashMap;
use std::io::BufRead;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::{fs, io};

use crate::cache::cache;

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
    force_rebuild: bool,
) -> Result<bool, std::io::Error> {
    if force_rebuild || !output.exists() {
        return Ok(false);
    }

    let toolchain = toolchains.get(&config.tools.default).ok_or_else(|| {
        io::Error::new(
            io::ErrorKind::NotFound,
            format!("Toolchain '{}' not found", config.tools.default)
        )
    })?;

    let input_strs: Vec<String> = inputs.iter()
        .map(|p| p.display().to_string())
        .collect();


    'tools: for (tool_name, tool) in toolchain {
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

        let mut cache_inputs: Vec<PathBuf> = inputs.iter().map(|p| p.to_path_buf()).collect();
        if let Some(dep_file_raw) = &tool.deps {
            let dep_file = replace_placeholders(&dep_file_raw, &replacements).unwrap(); // TODO
            if Path::new(&dep_file).exists() {
                let file = fs::File::open(&dep_file).unwrap(); // TODO
                let reader = io::BufReader::new(file);
                let mut lines: Vec<String> = reader.lines().collect::<Result<_, _>>()?;

                let format_name = tool.format.clone().unwrap(); // TODO
                let format = formats.get(&format_name).ok_or_else(|| {
                    io::Error::new(
                        io::ErrorKind::NotFound,
                        format!("Format '{}' not found", format_name)
                    )
                })?;

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
                                let n = trim.remove as usize;
                                if line.len() >= n {
                                    let new_len = line.len() - n;
                                    line.truncate(new_len);
                                } else {
                                    line.clear();
                                }
                            }
                        }

                        let s = line.trim();
                        if !s.is_empty() {
                            deps.push(s.to_string());
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

        if cache::check_built(&cache_inputs, &output.to_string_lossy(), cache) {
            return Ok(false);
        }

        let command = replace_placeholders(&tool.command, &replacements)
            .unwrap(); // TODO

        if let Some(raw_message) = &tool.message {
            let message = replace_placeholders(raw_message, &replacements).unwrap();
            println!("{}", message);
        }

        let mut parts = command.split_whitespace();
        if let Some(program) = parts.next() {
            let args: Vec<&str> = parts.collect();

            let status = Command::new(program).args(&args).status();

            cache::write_built(&cache_inputs, &output.to_string_lossy(), cache);
            if status?.success() {
                return Ok(true);
            } else {
                return Err(io::Error::new(
                    io::ErrorKind::Other,
                    format!("Compilation failed for output: {}", output.display()),
                ));
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
