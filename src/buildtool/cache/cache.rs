use std::fs::{self, OpenOptions};
use std::io::Write;
use std::path::{Path, PathBuf};
use serde_json::json;
use std::collections::HashMap;
use sha2::{Sha256, Digest};

pub fn compute_fingerprint(inputs: &[PathBuf]) -> String {
    let mut hasher = Sha256::new();

    for path in inputs {
        let content = fs::read(path).expect("Failed to read input file for fingerprint");
        hasher.update(&content);
    }

    let result = hasher.finalize();
    format!("{:x}", result)
}

pub fn check_built(cache_dir: &Path, inputs: &[PathBuf], output: &str) -> bool {
    let cache_file = cache_dir.join("cache.json");

    let build_cache: HashMap<String, String> = if cache_file.exists() {
        serde_json::from_str(&fs::read_to_string(&cache_file).unwrap()).unwrap()
    } else {
        HashMap::new()
    };

    let combined_fingerprint = compute_fingerprint(inputs);

    build_cache.get(output).map_or(false, |hash| hash == &combined_fingerprint)
}

pub fn write_built(cache_dir: &Path, inputs: &[PathBuf], output: &str) {
    let cache_file = cache_dir.join("cache.json");

    let mut build_cache: HashMap<String, String> = if cache_file.exists() {
        serde_json::from_str(&fs::read_to_string(&cache_file).unwrap()).unwrap()
    } else {
        HashMap::new()
    };

    let combined_fingerprint = compute_fingerprint(inputs);

    build_cache.insert(output.to_string(), combined_fingerprint);

    fs::write(cache_file, serde_json::to_string_pretty(&build_cache).unwrap()).unwrap();
}