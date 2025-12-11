use serde::Deserialize;
use std::collections::HashMap;

#[derive(Debug, Deserialize)]
pub struct Config {
    pub project: Project,

    pub tools: Tools,

    pub build: Build,

    #[serde(default)]
    pub targets: HashMap<String, Target>,

    #[serde(default)]
    pub flags: HashMap<String, Flags>,
}

#[derive(Debug, Deserialize)]
pub struct Project {
    pub name: String,
    pub version: Option<String>,
    pub description: Option<String>,

    #[serde(default)]
    pub internal_dump: bool,
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
    pub default_toolchain: String,
    pub default_mode: Option<String>,

    #[serde(default)]
    pub toolchains: Vec<String>,
    #[serde(default)]
    pub formats: Vec<String>,
}

#[derive(Clone, Debug, PartialEq, Eq, Deserialize)]
#[serde(untagged)]
pub enum OutputKind {
    Known(KnownOutputKind),
    Custom(String),
}

#[derive(Clone, Debug, PartialEq, Eq, Deserialize)]
#[serde(rename_all = "lowercase")]
pub enum KnownOutputKind {
    Executable,
    Object,
    #[serde(rename = "static-library")]
    StaticLibrary,

    Undefined,
}

#[derive(Clone, Debug, Deserialize)]
#[serde(untagged)]
pub enum StringOrVec {
    One(String),
    Many(Vec<String>),
}

impl StringOrVec {
    pub fn into_vec(&self) -> Vec<&String> {
        match self {
            StringOrVec::One(s) => vec![s],
            StringOrVec::Many(v) => v.iter().collect(),
        }
    }
}

#[derive(Debug, Deserialize)]
pub struct Target {
    pub description: Option<String>,
    pub message: Option<String>,
    #[serde(default)]
    pub depends: Vec<String>,
    #[serde(default)]
    pub before: Vec<String>,

    #[serde(default = "default_out_path")]
    pub out_path: String,

    pub for_each: Option<bool>,

    #[serde(default = "default_target_out")]
    pub out: OutputKind,
    
    #[serde(default = "default_target_path")]
    pub path: String,
    pub files: Option<StringOrVec>,

    pub run: Option<String>,
}

#[derive(Debug, Deserialize)]
pub struct Flags {
    pub build_dir: Option<String>,
    pub build_mode: Option<String>,
}

fn default_out_path() -> String { "{INPUT}".to_string() }

fn default_target_out() -> OutputKind { OutputKind::Known(KnownOutputKind::Undefined) }

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
        println!("  Default Toolchain: {}", self.tools.default_toolchain);
        if let Some(default_mode) = &self.tools.default_mode {
            println!("  Default Mode: {}", default_mode);
        }
        println!("  Toolchains:");
        for toolchain in &self.tools.toolchains {
            println!("  - {}", toolchain);
        }
        println!("  Formats:");
        for format in &self.tools.formats {
            println!("  - {}", format);
        }

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
            if let Some(files_sov) = &target.files {
                let files = files_sov.into_vec();
                println!("  Files: {:?}", files);
                println!("  Path: {}", target.path);
            }
        }
    }
}
