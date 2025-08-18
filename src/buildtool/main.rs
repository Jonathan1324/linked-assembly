use std::path::Path;
use std::fs;

mod args
{
    pub mod args;
}

mod yaml
{
    pub mod config;
    pub mod vars;
}

fn main()
{
    let args: Vec<String> = std::env::args().collect();

    if args.iter().any(|a| a == "-v" || a == "--version")
    {
        args::args::print_version();
        std::process::exit(0);
    }
    if args.iter().any(|a| a == "-h" || a == "--help")
    {
        args::args::print_help();
        std::process::exit(0);
    }

    let config_path = Path::new("build.yaml");

    if !config_path.exists()
    {
        eprintln!("Config file not found: {:?}", config_path);
        std::process::exit(1);
    }
    
    let yaml_str = fs::read_to_string(config_path)
        .expect("Failed to read YAML file");

    let config: yaml::config::Config = serde_yaml::from_str(&yaml_str)
        .expect("Failed to parse YAML");

    config.print_full();
}
