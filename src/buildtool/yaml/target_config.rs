use std::collections::HashMap;
use serde::Deserialize;

#[derive(Debug, Deserialize, Clone)]
pub struct Filters
{
    #[serde(default)]
    pub patterns: Vec<String>,

    #[serde(default)]
    pub exclude: Vec<String>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct File
{
    pub filters: Option<Filters>,

    pub toolchain: Option<String>,
}

#[derive(Debug, Deserialize, Clone)]
pub struct TargetFile
{
    #[serde(default)]
    pub files: HashMap<String, File>,
}