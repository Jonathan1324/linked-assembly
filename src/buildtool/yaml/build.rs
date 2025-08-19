use crate::target::files;
use crate::yaml::config;
use crate::yaml::vars::{expand_string, ExpandContext};
use crate::path::path::normalize_path;
use crate::yaml::target_config;
use std::collections::HashMap;
use std::path::{Path, PathBuf};

#[derive(Debug, Clone)]
pub struct Environment {
    pub description: String,

    pub toolchain: String,

    pub vars: HashMap<String, String>,
}

#[derive(Debug, Clone)]
pub struct Output {
    pub kind: String,
}

#[derive(Debug, Clone)]
pub struct Target {
    pub path: String,
    pub config: String,
    pub outputs: HashMap<String, Output>,
    pub env: String,
}

#[derive(Debug, Clone)]
pub struct Toolchain {
    pub description: String,
}

#[derive(Debug, Clone)]
pub struct Build {
    pub default_env: Environment,
    pub default_targets: Vec<String>,
    pub project_root: PathBuf,

    pub environments: HashMap<String, Environment>,
    pub targets: HashMap<String, Target>,
    pub toolchains: HashMap<String, Toolchain>,

    pub buildfile: config::BuildFile,
}

impl Build {

    pub fn copy_from_file(&mut self) {
        let  mut ctx = ExpandContext::new();
        ctx.default_env(&self.default_env)
           .env(&self.default_env)
           .project_root(&self.project_root)
           .path(&self.project_root);

        self.default_targets = self.buildfile.config.default_targets.iter()
                                   .map(|s| expand_string(&s, &ctx).unwrap())
                                   .collect();
        
        for (name, env) in &self.buildfile.environments {
            let mut new_vars = HashMap::new();

            for (k, v) in &env.vars {
                if k == "toolchain" || k == "description" {
                    panic!("The variable name '{}' is not allowed in env.vars!", k);
                }
                new_vars.insert(k.clone(), expand_string(&v, &ctx).unwrap());
            }

            let new_env = Environment {
                description: env.description.clone(),
                toolchain: expand_string(&env.toolchain, &ctx).unwrap(),
                vars: new_vars,
            };

            self.environments.insert(name.clone(), new_env);
        }

        for (name, target) in &self.buildfile.targets {
            let local_env_str = expand_string(&target.env, &ctx).unwrap();
            let local_env = self.environments
                                .get(&local_env_str)
                                .cloned().unwrap();
            let local_ctx = ctx.with_local_env(&local_env);
            
            let mut new_outputs = HashMap::new();

            for (o_name, output) in &target.outputs {
                let new_output = Output {
                    kind: expand_string(&output.kind, &local_ctx).unwrap(),
                };
                new_outputs.insert(o_name.clone(), new_output);
            }

            let new_target = Target {
                path: expand_string(&target.path, &local_ctx).unwrap(),
                config: expand_string(&target.config, &local_ctx).unwrap(),
                outputs: new_outputs,
                env: local_env_str,
            };

            self.targets.insert(name.clone(), new_target);
        }

        for (name, toolchain) in &self.buildfile.toolchains {
            let new_toolchain = Toolchain {
                description: expand_string(&toolchain.description, &ctx).unwrap(),
            };

            self.toolchains.insert(name.clone(), new_toolchain);
        }
    }

    pub fn set_full_paths(&mut self) {
        for (_name, target) in self.targets.iter_mut() {
            target.path = normalize_path(&target.path, &self.project_root);
            target.config = normalize_path(&target.config, Path::new(""));
        }
    }


    pub fn read_target_configs(&mut self) {
        for (name, target) in &self.targets {
            let config_path = Path::new(&target.path).join(&target.config);

            if !config_path.exists() {
                eprintln!("Config file of target {} not found: {:?}", name, config_path);
                std::process::exit(1);
            }

            let config_str = std::fs::read_to_string(&config_path)
                .expect("Failed to read target YAML file");

            let config: target_config::TargetFile = serde_yaml::from_str(&config_str)
                .expect("Failed to parse target YAML");

            let target_env = self.environments.get(&target.env).unwrap();

            let mut target = files::Target {
                targetfile: config,
                path: target.path.clone(),
                files: HashMap::new(),
                env: target_env,
            };

            target.get_files(self);

            println!("{:?}", target);
        }
    }


    pub fn print_full(&self) {
        println!("project_root: {}", self.project_root.display());

        println!("\n--- Config ---");
        println!("default_env: {}", self.buildfile.config.default_env.clone());

        if self.default_targets.is_empty() {
            println!("default_targets: (none)");
        } else {
            println!("default_targets:");
            for t in &self.default_targets {
                println!("  - {}", t);
            }
        }

        println!("\n--- Toolchains ---");
        for (name, tc) in &self.toolchains {
            println!("Toolchain: {}", name);
            println!("  description: {}", tc.description);
        }

        println!("\n--- Environments ---");
        for (name, env) in &self.environments {
            println!("Environment: {}", name);
            println!("  toolchain: {}", env.toolchain);
            if env.vars.is_empty() {
                println!("  vars: (none)");
            } else {
                println!("  vars:");
                for (k, v) in &env.vars {
                    println!("    {} = {}", k, v);
                }
            }
        }

        println!("\n--- Targets ---");
        for (name, target) in &self.targets {
            println!("Target: {}", name);
            println!("  path: {}", target.path);
            println!("  config: {}", target.config);
            println!("  env: {}", target.env);

            if target.outputs.is_empty() {
                println!("  outputs: (none)");
            } else {
                println!("  outputs:");
                for (name, out) in &target.outputs {
                    println!("    {}: {}", name, out.kind);
                }
            }
        }
    }
}
