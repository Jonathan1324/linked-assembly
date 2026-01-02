use std::env;

fn main() {
    let lib_dir = env::var("LIB_DIR").unwrap_or_else(|_| "libs".into());
    println!("cargo:rustc-link-search=native={}", lib_dir);

    println!("cargo:rustc-link-lib=static=core");
    println!("cargo:rustc-link-lib=static=rust");
}
