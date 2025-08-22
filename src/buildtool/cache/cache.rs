use std::fs::{self, canonicalize, OpenOptions};
use std::io::Write;
use std::path::{Path, PathBuf};
use serde_json::json;
use std::collections::HashMap;
use sha2::{Sha256, Digest};
use std::ffi::CString;
use base64::{engine::general_purpose, Engine as _};
use crate::c;

#[repr(transparent)]
pub struct CacheBuffer {
    ptr: u64,
}

impl CacheBuffer {
    pub fn parse_file<P: AsRef<Path>>(path: P) -> Option<Self> {
        let c_path = CString::new(path.as_ref().as_os_str().as_encoded_bytes()).ok()?;
        let ptr = unsafe { c::ParseCacheFile(c_path.as_ptr()) };
        if ptr == 0 {
            None
        } else {
            Some(CacheBuffer { ptr })
        }
    }

    pub fn write_file<P: AsRef<Path>>(&self, path: P) -> bool {
        let c_path = match CString::new(path.as_ref().as_os_str().as_encoded_bytes()) {
            Ok(s) => s,
            Err(_) => return false,
        };
        unsafe { c::WriteCacheFile(self.ptr, c_path.as_ptr()) };
        true
    }

    pub fn add(&self, name: &str, value: &str) -> bool {
        let c_name = match CString::new(name) {
            Ok(s) => s,
            Err(_) => return false,
        };
        let c_value = match CString::new(value) {
            Ok(s) => s,
            Err(_) => return false,
        };
        unsafe { c::AddToCache(self.ptr, c_name.as_ptr(), c_value.as_ptr()) };
        true
    }

    pub fn read(&self, name: &str) -> Option<String> {
        let c_name = CString::new(name).ok()?;
        let value_ptr = unsafe { c::ReadFromCache(self.ptr, c_name.as_ptr()) };
        if value_ptr.is_null() {
            None
        } else {
            unsafe { Some(std::ffi::CStr::from_ptr(value_ptr).to_string_lossy().into_owned()) }
        }
    }
}

impl Drop for CacheBuffer {
    fn drop(&mut self) {
        unsafe { c::FreeCacheBuffer(self.ptr); }
    }
}


// TODO: not as string but as raw bytes (also update c to have length)
pub fn hash_path(path: &str) -> String {
    let mut hasher = Sha256::new();
    hasher.update(path.as_bytes());
    let result = hasher.finalize();
    general_purpose::STANDARD.encode(result)
}

pub fn compute_fingerprint(inputs: &[PathBuf]) -> String {
    let mut hasher = Sha256::new();

    for path in inputs {
        let content = fs::read(path).expect("Failed to read input file for fingerprint");
        hasher.update(&content);
    }

    let result = hasher.finalize();
    general_purpose::STANDARD.encode(result)
}

pub fn check_built(cache_dir: &Path, inputs: &[PathBuf], output: &str, cache: &CacheBuffer) -> bool {
    let combined_fingerprint = compute_fingerprint(inputs);
    let output_hash = hash_path(output);

    match cache.read(&output_hash) {
        Some(value) => value == combined_fingerprint,
        None => false,
    }
}

pub fn write_built(cache_dir: &Path, inputs: &[PathBuf], output: &str, map_names: bool, cache: &CacheBuffer) {
    let combined_fingerprint = compute_fingerprint(inputs);
    let output_hash = hash_path(output);

    cache.add(&output_hash, &combined_fingerprint);

    let names_file = cache_dir.join("names.json");

    if map_names {
        let mut name_map: HashMap<String, String> = if names_file.exists() {
            serde_json::from_str(&fs::read_to_string(&names_file).unwrap()).unwrap()
        } else {
            HashMap::new()
        };

        name_map.insert(output_hash, output.to_string());
        fs::write(names_file, serde_json::to_string_pretty(&name_map).unwrap()).unwrap();
    }
}