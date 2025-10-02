use serde::Deserialize;
use std::collections::HashMap;

#[derive(Clone, Debug, Deserialize)]
pub struct Toolchain {
    pub tools: HashMap<String, Tool>,
    pub flags: HashMap<String, Flags>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Flags {
    #[serde(default)]
    pub default: Vec<String>,
    #[serde(default)]
    pub windows: Vec<String>,
    #[serde(default)]
    pub macos: Vec<String>,
    #[serde(default)]
    pub linux: Vec<String>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Tool {
    pub when: When,

    pub deps: Option<String>,
    pub format: Option<String>,

    pub flags: Option<String>,

    pub command: String,
    pub message: Option<String>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct When {
    pub ext: Option<Vec<String>>,
    pub out: crate::config::OutputKind,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Format {
    pub split: Option<String>,
    pub start: Option<FormatRule>,
    pub end: Option<FormatRule>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct FormatRule {
    #[serde(default)]
    pub ignore_lines: u32,
    pub trim: Option<Trim>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Trim {
    #[serde(rename = "if")]
    pub condition: String,
    pub remove: u32,
}

pub fn print_toolchains(toolchains: &HashMap<String, Toolchain>) {
	for (toolchain_name, toolchain) in toolchains {
        println!("{}: ", toolchain_name);
        println!(" Tools: ");
        for (tool_name, tool) in &toolchain.tools {
            println!("    {}: ", tool_name);
            println!("      Command: {}", tool.command);
            if let Some(deps) = &tool.deps {
                println!("      Deps: {}", deps);
            }
            if let Some(format) = &tool.format {
                println!("      Format: {}", format);
            }
            if let Some(flags) = &tool.flags {
                println!("      Flags: {}", flags);
            }
            if let Some(message) = &tool.message {
                println!("      Message: {}", message);
            }
            println!("      when:");
            println!("        out: {:?}", tool.when.out);
            if let Some(exts) = &tool.when.ext {
                if !exts.is_empty() {
                    println!("        ext:");
                    for ext in exts {
                        println!("        - {}", ext);
                    }
                }
            }
        }

        println!(" Flags: ");
        for (flag_name, flag) in &toolchain.flags {
            println!("    {}: ", flag_name);
            println!("      Default:");
            for default in &flag.default {
                println!("      - {}", default);
            }
            println!("      Windows:");
            for windows in &flag.windows {
                println!("      - {}", windows);
            }
            println!("      Linux:");
            for linux in &flag.linux {
                println!("      - {}", linux);
            }
            println!("      macOS:");
            for macos in &flag.macos {
                println!("      - {}", macos);
            }
        }
    }
}
