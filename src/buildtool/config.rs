use serde::Deserialize;
use std::collections::HashMap;

#[derive(Debug, Deserialize)]
pub struct Config {
    pub project: Project,

    pub tools: Tools,

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
pub struct Tools {
    pub default: String,
    pub file: String,
}

#[derive(Debug, PartialEq, Eq, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum OutputKind {
    Executable,
    Object,

    Custom,
}

#[derive(Debug, Deserialize)]
pub struct Target {
    pub description: Option<String>,
    pub message: Option<String>,
    #[serde(default)]
    pub depends: Vec<String>,

    #[serde(default = "default_target_out")]
    pub out: OutputKind,
    
    #[serde(default = "default_target_path")]
    pub path: String,
    pub files: Option<String>,

    pub run: Option<String>,
}

fn default_target_out() -> OutputKind { OutputKind::Custom }

fn default_target_path() -> String { ".".to_string() }

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

        println!("[TOOLS]");
        println!("  Default: {}", self.tools.default);
        println!("  File: {}", self.tools.file);

        for (name, target) in &self.targets {
            println!("[TARGETS.{}]", name);

            println!("  Out: {:?}", target.out);
            if let Some(description) = &target.description {
                println!("  Description: {}", description);
            }
            if let Some(message) = &target.message {
                println!("  Message: {}", message);
            }
            if !target.depends.is_empty() {
                println!("  Depends:");
                for dep in &target.depends {
                    println!("  - {}", dep);
                }
            }
            if let Some(command) = &target.run {
                println!("  Command: {}", command);
            }
            if let Some(files) = &target.files {
                println!("  Files: {}", files);
                println!("  Path: {}", target.path);
            }
        }
    }
}
