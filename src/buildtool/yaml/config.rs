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

    #[serde(default)]
    pub outputs: HashMap<String, Output>,

    pub env: String,
}

#[derive(Debug, Deserialize)]
pub struct Toolchain
{
    #[serde(default)]
    pub description: String,
}

#[derive(Debug, Deserialize)]
pub struct Environment {
    #[serde(default)]
    pub description: String,

    pub toolchain: String,

    /// Key-Value
    #[serde(default)]
    pub vars: HashMap<String, String>,
}

#[derive(Debug, Deserialize)]
pub struct Config
{
    #[serde(default)]
    pub toolchains: HashMap<String, Toolchain>,

    #[serde(default)]
    pub environments: HashMap<String, Environment>,

    #[serde(default)]
    pub targets: HashMap<String, Target>,
}

impl Target
{
    pub fn expanded(&self, envs: &std::collections::HashMap<String, Environment>) -> Target
    {
        let mut new = self.clone();
        if let Some(env) = envs.get(&self.env)
        {
            new.path = crate::yaml::vars::expand_string(&self.path, &env.vars);
            new.config = crate::yaml::vars::expand_string(&self.config, &env.vars);

            new.outputs = self.outputs.iter()
                .map(|(name, output)| {
                    let mut out = output.clone();
                    // Output
                    out.kind = crate::yaml::vars::expand_string(&output.kind, &env.vars);
                    (name.clone(), out)
                })
                .collect();
        }
        new
    }
}



impl Config
{
    pub fn print_full(&self)
    {
        println!("--- Toolchains ---");
        for (name, tc) in &self.toolchains
        {
            println!("Toolchain: {}", name);
            println!("  description: {}", tc.description);
        }

        println!("\n--- Environments ---");
        for (name, env) in &self.environments
        {
            println!("Environment: {}", name);
            println!("  toolchain: {}", env.toolchain);
            if env.vars.is_empty()
            {
                println!("  vars: (none)");
            }
            else
            {
                println!("  vars:");
                for (k, v) in &env.vars
                {
                    println!("    {} = {}", k, v);
                }
            }
        }

        println!("\n--- Targets ---");
        for (name, _target) in &self.targets
        {
            let target = _target.expanded(&self.environments);
            println!("Target: {}", name);
            println!("  path: {}", target.path);
            println!("  config: {}", target.config);
            println!("  env: {}", target.env);

            if target.outputs.is_empty()
            {
                println!("  outputs: (none)");
            }
            else
            {
                println!("  outputs:");
                for (name, out) in &target.outputs {
                    println!("    {}: {}", name, out.kind);
                }
            }
        }
    }
}