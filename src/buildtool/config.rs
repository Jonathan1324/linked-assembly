use serde::Deserialize;
use std::collections::HashMap;

#[derive(Debug, Deserialize)]
pub struct Config {
    pub project: Project,


    pub build: Build,

    #[serde(default)]
    pub targets: HashMap<String, Target>,
}

#[derive(Debug, Deserialize)]
pub struct Project {
    pub name: String,
    pub version: Option<String>,
    pub description: Option<String>,
}

#[derive(Debug, Deserialize)]
pub struct Build {
    #[serde(default = "default_build_dir")]
    pub dir: String,

    pub default_target: Option<String>,
}

fn default_build_dir() -> String { "build".to_string() }

#[derive(Debug, Deserialize)]
pub struct Target {
    pub description: Option<String>,
    pub run: Option<String>,
}

impl Config {
    pub fn print(self) {
        println!("[PROJECT]");
        println!("  Project Name: {}", self.project.name);
        if let Some(version) = &self.project.version {
            println!("  Version: {}", version);
        }
        if let Some(description) = &self.project.description {
            println!("  Description: {}", description);
        }

        println!("[BUILD]");
        println!("  Build directory: {}", self.build.dir);
        if let Some(default_target) = &self.build.default_target {
            println!("  Default target: {}", default_target);
        }

        for (name, target) in &self.targets {
            println!("[TARGETS.{}]", name);

            if let Some(description) = &target.description {
                println!("  Description: {}", description);
            }
            if let Some(command) = &target.run {
                println!("  Command: {}", command);
            }
        }
    }
}
