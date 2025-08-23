use std::collections::HashMap;
use serde::Deserialize;

#[derive(Debug, Deserialize, Clone)]
pub struct Filters {
    #[serde(default)]
    pub patterns: Vec<String>,

    #[serde(default)]
    pub exclude: Vec<String>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Files {
    pub filters: Option<Filters>,

    pub target: String,

    #[serde(default)]
    pub at_once: bool,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Output {
    pub path: String,
    
    #[serde(rename = "type")]
    pub kind: String,
}

#[derive(Debug, Deserialize, Clone)]
pub struct Target {
    pub toolchain: Option<String>,

    #[serde(default)]
    pub depends: Vec<String>,

    pub outputs: HashMap<String, Output>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct TargetFile {
    #[serde(default)]
    pub files: HashMap<String, Files>,

    pub targets: HashMap<String, Target>,
}
