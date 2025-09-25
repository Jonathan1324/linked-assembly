use std::path::{Path, PathBuf};
use std::process::Command;
use std::io;

pub fn execute(inputs: Vec<PathBuf>, output: &Path) -> Result<(), std::io::Error> {
    let input_strs: Vec<String> = inputs.iter()
        .map(|p| p.display().to_string())
        .collect();

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