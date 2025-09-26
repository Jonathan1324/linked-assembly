use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::io;

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

pub fn execute(inputs: Vec<&Path>, output: &Path, out: &crate::config::OutputKind, config: &crate::config::Config, toolchains: &HashMap<String, crate::tools::tools::Toolchain>) -> Result<(), std::io::Error> {
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

        let mut replacements = HashMap::new();
        replacements.insert("input".to_string(), input_strs.join(" "));
        replacements.insert("output".to_string(), output.to_string_lossy().to_string());

        let command = replace_placeholders(&tool.command, &replacements)
            .unwrap(); // TODO

        if let Some(raw_message) = &tool.message {
            let message = replace_placeholders(raw_message, &replacements).unwrap();
            println!("{}", message);
        }

        println!("{command}");

        let mut parts = command.split_whitespace();
        if let Some(program) = parts.next() {
            let args: Vec<&str> = parts.collect();

            let status = Command::new(program).args(&args).status();

            if status?.success() {
                return Ok(());
            } else {
                return Err(io::Error::new(
                    io::ErrorKind::Other,
                    format!("Compilation failed for output: {}", output.display()),
                ));
            }
        }

        // Fitting tool found
        break;
    }

    return Err(io::Error::new(
        io::ErrorKind::Other,
        format!("Didn't find any fitting tool"),
    ));
}