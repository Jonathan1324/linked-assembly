use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::io;

pub fn execute(inputs: Vec<PathBuf>, output: &Path, config: &crate::config::Config, toolchains: &HashMap<String, crate::tools::tools::Toolchain>) -> Result<(), std::io::Error> {
    let toolchain = toolchains.get(&config.tools.default).ok_or_else(|| {
        io::Error::new(
            io::ErrorKind::NotFound,
            format!("Toolchain '{}' not found", config.tools.default)
        )
    })?;

    let input_strs: Vec<String> = inputs.iter()
        .map(|p| p.display().to_string())
        .collect();


    for (tool_name, tool) in toolchain {
      
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