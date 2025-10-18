use std::fs;
use std::path::{Path, PathBuf};
use sha2::{Sha256, Digest};
use std::ffi::CString;
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

    pub fn write_file<P: AsRef<Path>>(&self, path: P, clean: bool) -> bool {
        let c_path = match CString::new(path.as_ref().as_os_str().as_encoded_bytes()) {
            Ok(s) => s,
            Err(_) => return false,
        };
        if clean {
            unsafe  { c::CleanCache(self.ptr); };
        }
        unsafe { c::WriteCacheFile(self.ptr, c_path.as_ptr()) };
        true
    }

    pub fn add(&self, name: &[u8], value: &[u8]) -> bool {
        unsafe {
            c::AddToCache(
                self.ptr,
                name.as_ptr() as *const libc::c_char,
                name.len() as u64,
                value.as_ptr() as *const libc::c_char,
                value.len() as u64,
            );
        }
        true
    }

    pub fn read(&self, name: &[u8]) -> Option<Vec<u8>> {
        let mut value_len: u64 = 0;

        unsafe {
            let value_ptr = c::ReadFromCache(
                self.ptr,
                name.as_ptr() as *const libc::c_char,
                name.len() as u64,
                &mut value_len as *mut u64,
            );

            if value_ptr.is_null() || value_len == 0 {
                None
            } else {
                Some(std::slice::from_raw_parts(value_ptr as *const u8, value_len as usize).to_vec())
            }
        }
    }
}

impl Drop for CacheBuffer {
    fn drop(&mut self) {
        unsafe { c::FreeCacheBuffer(self.ptr); }
    }
}

pub fn compute_fingerprint(inputs: &[PathBuf]) -> Vec<u8> {
    let mut hasher = Sha256::new();

    for path in inputs {
        let content = fs::read(path).expect("Failed to read input file for fingerprint");
        hasher.update(&content);
    }

    let result = hasher.finalize();
    result.to_vec()
}

pub fn check_built(inputs: &[PathBuf], output: &str, cache: &CacheBuffer) -> bool {
    let combined_fingerprint = compute_fingerprint(inputs);
    match cache.read(&output.as_bytes()) {
        Some(value) => value == combined_fingerprint,
        None => false,
    }
}

pub fn write_built(inputs: &[PathBuf], output: &str, cache: &CacheBuffer) {
    let combined_fingerprint = compute_fingerprint(inputs);
    cache.add(&output.as_bytes(), &combined_fingerprint);
}
