use std::collections::HashSet;
use std::path::{Path, PathBuf};
use std::fs;
use std::env;

pub mod config;
pub mod execute;
pub mod target;
pub mod c;

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

    let build_dir = env::current_dir().unwrap().join(config.build.dir.clone());

    let mut executed = HashSet::new();
    for target_name in targets {
        target::execute_target(&target_name, &config, &mut executed, &build_dir);
    }

    config.print();

}