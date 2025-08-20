use std::path::Path;
use std::fs;
use yaml::config::BuildFile;
use yaml::build::Build;
use std::env;

mod args {
    pub mod args;
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
        vars: env.vars.clone(),
    };

    let mut build = Build {
        default_env: std::sync::Arc::new(default_env),
        default_targets: Vec::new(),
        project_root: env::current_dir().expect("Failed to get current directory"),
        environments: std::collections::HashMap::new(),
        targets: std::collections::HashMap::new(),
        toolchains: std::collections::HashMap::new(),
        buildfile,
    };

    build.copy_from_file();
    build.resolve_targets();

    build.build();

    // print debug info
    // build.print_full();
}
