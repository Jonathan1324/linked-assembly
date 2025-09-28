use std::io;
use std::collections::HashMap;
use crate::tools::tools::{Format, Toolchain};

pub fn get_toolchain<'a>(
    name: &str,
    toolchains: &'a HashMap<String, Toolchain>,
) -> Result<&'a Toolchain, io::Error> {
    if let Some(default) = name.strip_prefix("default.") {
        // TODO
    }

    toolchains.get(name).ok_or_else(|| {
        io::Error::new(
            io::ErrorKind::NotFound,
            format!("Toolchain '{}' not found", name)
        )
    })
}

pub fn get_format<'a>(
    name: &str,
    formats: &'a HashMap<String, Format>,
) -> Result<&'a Format, io::Error> {
    if let Some(default) = name.strip_prefix("default.") {
        // TODO
    }
    
    formats.get(name).ok_or_else(|| {
        io::Error::new(
            io::ErrorKind::NotFound,
            format!("Format '{}' not found", name)
        )
    })
}
