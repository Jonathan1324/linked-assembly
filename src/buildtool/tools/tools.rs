use serde::Deserialize;
use std::collections::HashMap;

#[derive(Debug, Deserialize)]
pub struct Tool {
    pub when: When,
    pub message: Option<String>,
}

#[derive(Debug, Deserialize)]
pub struct When {
    pub ext: Option<Vec<String>>,
    pub out: crate::config::OutputKind,
}

pub type Toolchain = HashMap<String, Tool>;

pub fn print_toolchains(toolchains: &HashMap<String, Toolchain>) {
	for (toolchain_name, toolchain) in toolchains {
        println!("{}: ", toolchain_name);
        for (tool_name, tool) in toolchain {
            println!("  {}: ", tool_name);
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
