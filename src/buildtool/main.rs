use std::path::Path;
use std::fs;
use yaml::config::BuildFile;
use yaml::build::Build;
use std::env;

mod args {
    pub mod args;
}

mod path {
    pub mod path;
}

mod yaml {
    pub mod build;
    pub mod config;
    pub mod vars;
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

    let mut build = Build {
        default_env: None,
        default_targets: Vec::new(),
        environments: std::collections::HashMap::new(),
        targets: std::collections::HashMap::new(),
        toolchains: std::collections::HashMap::new(),
        buildfile,
    };

    let project_root = env::current_dir().expect("Failed to get current directory");

    build.set_default_env();
    build.set_default_targets();
    build.copy_envs();
    build.copy_targets();
    build.copy_toolchains();
    build.set_full_paths(&project_root);

    // print debug info
    build.print_full();

    for target_name in &build.default_targets {
        let target = build.targets.get(target_name).cloned().unwrap();
        let config_file = target.path.clone() + "/" + &target.config;

        // TODO: currently just runs the shell file
        let status = std::process::Command::new("sh")
            .arg(&config_file)
            .status()
            .expect("Failed to execute shell script");

        if !status.success() {
            eprintln!("Shell script failed");
        }
    }
}
