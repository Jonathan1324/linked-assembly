use crate::config;
use crate::execute;
use glob::glob;
use std::collections::HashMap;
use std::process::Command;
use std::fs;
use std::env;
use std::path::{Path, PathBuf};

pub fn execute_target(name: &str, config: &config::Config, toolchains: &HashMap<String, crate::tools::tools::Toolchain>, executed: &mut std::collections::HashSet<String>, build_dir: &Path) {
    if executed.contains(name) {
        return
    }

    if let Some(target) = config.targets.get(name) {
        if let Some(message) = &target.message {
            println!("{}", message);
        }
        
        for dep in &target.depends {
            execute_target(dep, config, toolchains, executed, build_dir);
        }

        if let Some(files) = &target.files {
            // files
            let target_path = if Path::new(&target.path).is_absolute() {
                PathBuf::from(&target.path)
            } else {
                env::current_dir()
                    .unwrap()
                    .join(target.path.clone())
            };

            let glob_pattern = format!("{}/{}", target_path.display(), files);
            for entry in glob(&glob_pattern).unwrap() {
                if let Ok(full_path) = entry {
                    let path = full_path.strip_prefix(&target_path).unwrap();
                    
                    //TODO: dynamically
                    let mut output_path = build_dir.join(path);
                    let mut file_name = output_path.file_name().unwrap().to_os_string();
                    file_name.push(".o");
                    output_path.set_file_name(file_name);

                    let inputs = vec![full_path.clone()];

                    if let Some(parent) = output_path.parent() {
                        fs::create_dir_all(parent).unwrap();
                    }

                    let status = execute::execute(inputs, &output_path, &target.out, config, toolchains);
                    println!("File: {} -> {}", full_path.display(), output_path.display());
                }
            }
            
        } else if let Some(command) = &target.run {
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
    } else {
        println!("Skipping unknown target: {}", name);
    }
}