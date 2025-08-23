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
pub struct Trim {
    #[serde(rename = "if")]
    pub condition: String,
    pub remove: u32,
}

#[derive(Debug, Deserialize, Clone)]
pub struct FormatRule {
    #[serde(default)]
    pub ignore_lines: u32,
    #[serde(default)]
    pub trim: Option<Trim>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Format {
    pub start: Option<FormatRule>,
    pub end: Option<FormatRule>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct ToolWhen {
    pub ext: Option<Vec<String>>,
    #[serde(rename = "type")]
    pub kind: String,
    pub out: String,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Tool {
    #[serde(rename = "type")]
    pub kind: String,

    pub when: ToolWhen,

    pub dep_path: Option<String>,
    pub dep_format: Option<String>,

    #[serde(default)]
    pub combine_inputs: bool,

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
    pub formats: HashMap<String, Format>,

    #[serde(default)]
    pub toolchains: HashMap<String, Toolchain>,

    #[serde(default)]
    pub environments: HashMap<String, Environment>,

    #[serde(default)]
    pub targets: HashMap<String, Target>,
}