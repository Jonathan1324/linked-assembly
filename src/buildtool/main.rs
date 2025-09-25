use std::path::Path;
use std::fs;

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

    println!("[PROJECT]");
    println!("  Project Name: {}", config.project.name);
    if let Some(version) = config.project.version {
        println!("  Version: {}", version);
    }
    if let Some(description) = config.project.description {
        println!("  Description: {}", description);
    }

    println!("[BUILD]");
    println!("  Build directory: {}", config.build.dir);
    if let Some(default_target) = config.build.default_target {
        println!("  Default target: {}", default_target);
    }

    for (name, target) in config.targets {
        println!("[TARGETS.{}]", name);

        if let Some(description) = target.description {
            println!("  description: {}", description);
        }
    }

}