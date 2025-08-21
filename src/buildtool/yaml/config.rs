use std::collections::HashMap;
use serde::Deserialize;

#[derive(Debug, Deserialize, Clone)]
pub struct Output {
    #[serde(rename = "type")]
    pub kind: String,       // "executable", "static-library", etc.
}

#[derive(Debug, Deserialize, Clone)]
pub struct Target {
    pub path: String,
    pub config: String,
    pub target: Option<String>,

    #[serde(default)]
    pub outputs: HashMap<String, Output>,

    pub env: String,
}

#[derive(Debug, Deserialize, Clone)]
pub struct ToolWhen {
    #[serde(default)]
    pub ext: Vec<String>,
    
    #[serde(rename = "type")]
    pub kind: String,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Tool {
    #[serde(rename = "type")]
    pub kind: String,

    pub when: ToolWhen,

    pub command: String,
    #[serde(default)]
    pub message: String,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Toolchain {
    #[serde(default)]
    pub description: String,

    pub tools: HashMap<String, Tool>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Environment {
    #[serde(default)]
    pub description: String,

    pub toolchain: String,

    pub build_dir: String,

    /// Key-Value
    #[serde(default)]
    pub vars: HashMap<String, String>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Config {
    pub default_env: String,

    #[serde(default)]
    pub default_targets: Vec<String>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct BuildFile {
    pub config: Config,

    #[serde(default)]
    pub toolchains: HashMap<String, Toolchain>,

    #[serde(default)]
    pub environments: HashMap<String, Environment>,

    #[serde(default)]
    pub targets: HashMap<String, Target>,
}