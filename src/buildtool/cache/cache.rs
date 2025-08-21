use std::fs::{self, OpenOptions};
use std::io::Write;
use std::path::Path;
use serde_json::json;
use std::collections::HashMap;

pub fn add_to_cache(cache_file: &Path, output_name: &str) {
    let mut file = OpenOptions::new()
        .append(true)
        .create(true)
        .open(cache_file)
        .unwrap();
    writeln!(file, "{}", output_name).unwrap();
}

pub fn check_built(cache_dir: &Path, output: &str) -> bool {
    let cache_file = cache_dir.join("cache.json");

    let mut build_cache: HashMap<String, bool> = if cache_file.exists() {
        serde_json::from_str(&fs::read_to_string(&cache_file).unwrap()).unwrap()
    } else {
        HashMap::new()
    };

    let already_built = build_cache.get(output).copied().unwrap_or(false);

    build_cache.insert(output.to_string(), true);

    fs::write(cache_file, serde_json::to_string_pretty(&build_cache).unwrap()).unwrap();

    !already_built
}