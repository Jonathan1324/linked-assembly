use crate::config;
use crate::execute;
use glob::glob;
use std::collections::HashMap;
use std::process::Command;
use std::fs;
use std::env;
use std::path::{Path, PathBuf};

pub fn execute_target(
    name: &str,
    config: &config::Config,
    toolchains: &HashMap<String, crate::tools::tools::Toolchain>,
    executed: &mut HashMap<String, Vec<PathBuf>>,
    build_dir: &Path
) -> Vec<PathBuf> {
    if let Some(existing_outputs) = executed.get(name) {
        return existing_outputs.clone();
    }

    let mut outputs = Vec::new();

    if let Some(target) = config.targets.get(name) {
        if let Some(message) = &target.message {
            println!("{}", message);
        }

        let mut inputs = Vec::new();
        
        for dep in &target.depends {
            let dep_outputs = execute_target(dep, config, toolchains, executed, build_dir);
            inputs.extend(dep_outputs);
        }

        if let Some(files) = &target.files {
            let target_path = if Path::new(&target.path).is_absolute() {
                PathBuf::from(&target.path)
            } else {
                env::current_dir().unwrap().join(&target.path)
            };

            let glob_pattern = format!("{}/{}", target_path.display(), files);
            for entry in glob(&glob_pattern).unwrap() {
                if let Ok(full_path) = entry {
                    inputs.push(full_path);
                }
            }
        }

        if !inputs.is_empty() {
            for input in &inputs {
                let output_path = build_dir.join(input.strip_prefix(&input.parent().unwrap()).unwrap());
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

                let mut output_path = output_path.clone();
                output_path.set_file_name(file_name);

                if let Some(parent) = output_path.parent() {
                    fs::create_dir_all(parent).unwrap();
                }

                let temp_inputs: Vec<&Path> = vec![input];

                execute::execute(temp_inputs, &output_path, &target.out, config, toolchains);

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

    outputs
}