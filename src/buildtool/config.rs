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
}
