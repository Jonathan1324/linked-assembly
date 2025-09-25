use std::path::Path;
use std::fs;
use std::env;
use std::process::Command;

pub mod config;

fn main() {

    let config_path = Path::new("build.toml");
    if !config_path.exists() {
        eprintln!("Error: build.toml not found!");
        std::process::exit(1);
    }

    let config_content = fs::read_to_string(config_path).unwrap_or_else(|err| {
        eprintln!("Error: Failed to read build.toml: {}", err);
        std::process::exit(1);
    });

    let config: config::Config = toml::from_str(&config_content).unwrap_or_else(|err| {
        eprintln!("Error: Failed to parse build.toml: {}", err);
        std::process::exit(1);
    });

    let mut targets = Vec::new();
    for arg in env::args().skip(1) {
        targets.push(arg);
    }

    if targets.is_empty() && let Some(default_target) = &config.build.default_target {
        targets.push(default_target.clone());
    }

    for target_name in targets {
        if let Some(target) = config.targets.get(&target_name) {
            if let Some(command) = &target.run {
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
            println!("Skipping unknown target: {}", target_name);
        }
    }

    config.print();

}