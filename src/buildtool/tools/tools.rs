use serde::Deserialize;
use std::collections::HashMap;

#[derive(Debug, Deserialize)]
pub struct Tool {
    pub when: When,
    pub command: String,
    pub message: Option<String>,
    pub deps: Option<String>,
    pub format: Option<String>,
}

#[derive(Debug, Deserialize)]
pub struct When {
    pub ext: Option<Vec<String>>,
    pub out: crate::config::OutputKind,
}

pub type Toolchain = HashMap<String, Tool>;

#[derive(Debug, Deserialize)]
pub struct Format {
    pub start: Option<FormatRule>,
    pub end: Option<FormatRule>,
}

#[derive(Debug, Deserialize)]
pub struct FormatRule {
    #[serde(default)]
    pub ignore_lines: u32,
    pub trim: Option<Trim>,
}

#[derive(Debug, Deserialize)]
pub struct Trim {
    #[serde(rename = "if")]
    pub condition: String,
    pub remove: u32,
}

pub fn print_toolchains(toolchains: &HashMap<String, Toolchain>) {
	for (toolchain_name, toolchain) in toolchains {
        println!("{}: ", toolchain_name);
        for (tool_name, tool) in toolchain {
            println!("  {}: ", tool_name);
            println!("    Command: {}", tool.command);
            if let Some(deps) = &tool.deps {
                println!("    Deps: {}", deps);
            }
            if let Some(message) = &tool.message {
                println!("    Message: {}", message);
            }
            println!("    when:");
            println!("      out: {:?}", tool.when.out);
            if let Some(exts) = &tool.when.ext {
                if !exts.is_empty() {
                    println!("      ext:");
                    for ext in exts {
                        println!("      - {}", ext);
                    }
                }
            }
        }
    }
}
