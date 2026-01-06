use std::env;
use std::fs;
use std::path::Path;

fn main() {
    let lib_dir = env::var("LIB_DIR").unwrap_or_else(|_| "libs".into());
    let lib_path = Path::new(&lib_dir);
    if !lib_path.exists() {
        panic!("Library directory '{}' does not exist", lib_dir);
    }

    println!("cargo:rustc-link-search=native={}", lib_dir);

    println!("cargo:rerun-if-env-changed=LIB_DIR");
    println!("cargo:rerun-if-env-changed=STATIC_LIBS");

    let libs = env::var("STATIC_LIBS").unwrap_or_else(|_| "".into());
    for lib in libs.split(',') {
        let lib = lib.trim();
        if !lib.is_empty() {
            println!("cargo:rustc-link-lib=static={}", lib);

            let lib_file = lib_path.join(format!("lib{}.a", lib));
            if !lib_file.exists() {
                panic!("Library file '{}' not found", lib_file.display());
            }

            println!("cargo:rerun-if-changed={}", lib_file.display());
        }
    }

    for entry in fs::read_dir(lib_path).unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        if path.is_file() {
            println!("cargo:rerun-if-changed={}", path.display());
        }
    }
}
