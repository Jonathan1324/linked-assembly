use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::io;

pub fn execute(inputs: Vec<PathBuf>, output: &Path, out: &crate::config::OutputKind, config: &crate::config::Config, toolchains: &HashMap<String, crate::tools::tools::Toolchain>) -> Result<(), std::io::Error> {
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

        println!("{tool_name}");

        // Fitting tool found
        break;
    }

    let status = Command::new("gcc")
        .arg("-c")
        .args(&input_strs)
        .arg("-o")
        .arg(output)
        .status()?;

    if status.success() {
        Ok(())
    } else {
        Err(io::Error::new(
            io::ErrorKind::Other,
            format!("Compilation failed for output: {}", output.display()),
        ))
    }
}