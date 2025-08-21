use std::fs::{self, OpenOptions};
use std::io::Write;
use std::path::{Path, PathBuf};
use serde_json::json;
use std::collections::HashMap;
use sha2::{Sha256, Digest};

pub fn parse_deps(file: &fs::File, format: &String) -> Vec<String> {
    let lines: Vec<&str> = format.lines().collect();
    println!("'{}'", format);
    Vec::new()
}

pub fn create_fingerprint(content: &String) -> String {
    let mut hasher = Sha256::new();
    hasher.update(content.as_bytes());
    let result = hasher.finalize();
    format!("{:x}", result)
}

pub fn check_built(cache_dir: &Path, inputs: &Vec<PathBuf>, output: &str) -> bool {
    let cache_file = cache_dir.join("cache.json");

    let mut build_cache: HashMap<String, String> = if cache_file.exists() {
        serde_json::from_str(&fs::read_to_string(&cache_file).unwrap()).unwrap()
    } else {
        HashMap::new()
    };

    let mut combined_fingerprint = String::new();
    for path in inputs {
        let content = fs::read_to_string(path).unwrap();
        combined_fingerprint.push_str(&create_fingerprint(&content));
    }

    let already_built = build_cache.get(output).map_or(false, |hash| hash == &combined_fingerprint);;

    build_cache.insert(output.to_string(), combined_fingerprint);
    fs::write(cache_file, serde_json::to_string_pretty(&build_cache).unwrap()).unwrap();

    already_built
}