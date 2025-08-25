use std::path::Path;
use std::fs;
use yaml::config::BuildFile;
use yaml::build::Build;
use std::env;

mod args {
    pub mod args;
}

mod cache {
    pub mod cache;
}

mod target {
    pub mod files;
}

mod path {
    pub mod path;
}

mod yaml {
    pub mod build;
    pub mod config;
    pub mod vars;
    pub mod target_config;
}

pub mod c;

fn main() {
    let args: Vec<String> = std::env::args().collect();

    if args.iter().any(|a| a == "-v" || a == "--version") {
        args::args::print_version();
        std::process::exit(0);
    }
    if args.iter().any(|a| a == "-h" || a == "--help") {
        args::args::print_help();
        std::process::exit(0);
    }

    let config_path = Path::new("build.yaml");

    if !config_path.exists() {
        eprintln!("Config file not found: {:?}", config_path);
        std::process::exit(1);
    }
    
    let yaml_str = fs::read_to_string(config_path)
        .expect("Failed to read YAML file");

    let buildfile: BuildFile = serde_yaml::from_str(&yaml_str)
        .expect("Failed to parse YAML");

    let env = buildfile.environments
                       .get(&buildfile.config.default_env)
                       .cloned().unwrap();
    
    let default_env = yaml::build::Environment {
        description: env.description.clone(),
        toolchain: env.toolchain.clone(),
        build_dir: env.build_dir.clone(),
        vars: env.vars.clone(),
    };

    let mut os = "unknown".to_string();
    if cfg!(target_os = "windows") {
        os = "windows".to_string();
    } else if cfg!(target_os = "linux") {
        os = "linux".to_string();
    } else if cfg!(target_os = "macos") {
        os = "macos".to_string();
    } else if cfg!(target_os = "freebsd") {
        os = "freebsd".to_string();
    } else if cfg!(target_os = "netbsd") {
        os = "netbsd".to_string();
    } else if cfg!(target_os = "ios") {
        os = "ios".to_string();
    } else if cfg!(target_os = "android") {
        os = "android".to_string();
    }

    let mut build = Build {
        default_env: std::sync::Arc::new(default_env),
        default_targets: Vec::new(),
        project_root: env::current_dir().expect("Failed to get current directory"),
        environments: std::collections::HashMap::new(),
        targets: std::collections::HashMap::new(),
        toolchains: std::collections::HashMap::new(),
        formats: std::collections::HashMap::new(),
        buildfile,
        os,
    };

    build.copy_from_file();
    build.resolve_targets();

    build.build();

    if args.iter().any(|a| a == "-d" || a == "--debug") {
        build.print_full();
    }
}
