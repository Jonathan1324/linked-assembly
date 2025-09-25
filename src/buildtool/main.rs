use std::path::Path;

fn main() {

    let config_path = Path::new("build.toml");

    if !config_path.exists() {
        eprintln!("Error: build.toml not found!");
        std::process::exit(1);
    }

}