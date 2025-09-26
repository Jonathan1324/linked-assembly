use std::collections::{HashMap, HashSet};
use std::path::{Path, PathBuf};
use std::fs;
use std::env;
use serde_yaml;

use crate::tools::tools::print_toolchains;

mod tools {
    pub mod tools;
}

mod cache {
    pub mod cache;
}

pub mod config;
pub mod execute;
pub mod target;

pub mod c;

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.iter().any(|a| a == "-v" || a == "--version") {
        unsafe { c::printVersion(); }
        std::process::exit(0);
    }
    if args.iter().any(|a| a == "-h" || a == "--help") {
        //args::args::print_help();
        std::process::exit(0);
    }

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
        if !arg.starts_with("-") {
            targets.push(arg);
        }
    }

    if targets.is_empty() && let Some(default_target) = &config.build.default_target {
        targets.push(default_target.clone());
    }

    let tools_file = &config.tools.file;
    let tools_content = fs::read_to_string(&tools_file).unwrap_or_else(|err | {
        eprintln!("Error: Failed to read {}: {}", tools_file, err);
        std::process::exit(1);
    });
    let toolchains: HashMap<String, tools::tools::Toolchain> = serde_yaml::from_str(&tools_content).unwrap_or_else(|err | {
        eprintln!("Error: Failed to parse {}: {}", tools_file, err);
        std::process::exit(1);
    });

    let formats_file = &config.tools.formats;
    let formats_content = fs::read_to_string(&formats_file).unwrap_or_else(|err | {
        eprintln!("Error: Failed to read {}: {}", formats_file, err);
        std::process::exit(1);
    });
    let formats: HashMap<String, tools::tools::Format> = serde_yaml::from_str(&formats_content).unwrap_or_else(|err | {
        eprintln!("Error: Failed to parse {}: {}", tools_file, err);
        std::process::exit(1);
    });

    let cache_dir = PathBuf::from("__lbtcache__");
    if !cache_dir.exists() {
        fs::create_dir_all(&cache_dir).unwrap();
    }
    let cache_file = cache_dir.join("cache.dat");

    let cache = cache::cache::CacheBuffer::parse_file(&cache_file).unwrap_or_else(|| {
        eprintln!("Error: Couldn't create cache buffer");
        std::process::exit(1);
    });

    let build_dir = env::current_dir().unwrap().join(config.build.dir.clone());
    let mut executed = HashMap::new();
    for target_name in targets {
        let result = target::execute_target(&target_name, &config, &toolchains, &formats, &mut executed, &build_dir, &cache);
        if result.is_err() {
            eprintln!("Target {} failed", target_name);
            break;
        }
    }

    cache.write_file(&cache_file);

    if config.project.internal_dump {
        println!("----------TOOLS----------");
        tools::tools::print_toolchains(&toolchains);

        println!("----------CONFIG----------");
        config.print();
    }

}