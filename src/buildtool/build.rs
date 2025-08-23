use std::env;

fn main() {
    // LIB_DIR kann dynamisch über ENV gesetzt werden
    let lib_dir = env::var("LIB_DIR").unwrap_or_else(|_| "libs".into());
    println!("cargo:rustc-link-search=native={}", lib_dir);

    // Statische Libraries einbinden
    println!("cargo:rustc-link-lib=static=core");
    println!("cargo:rustc-link-lib=static=rust");
}
