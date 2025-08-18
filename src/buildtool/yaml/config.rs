use serde::Deserialize;

#[derive(Debug, Deserialize)]
pub struct Toolchain
{
    #[serde(default)]
    pub description: String,
}

#[derive(Debug, Deserialize)]
pub struct Config
{
    #[serde(default)]
    pub toolchains: std::collections::HashMap<String, Toolchain>, 
}
