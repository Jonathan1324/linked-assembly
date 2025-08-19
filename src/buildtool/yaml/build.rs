use crate::yaml::config;
use crate::yaml::vars::expand_string;
use crate::path::path::normalize_path;
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
    pub default_env: Option<Environment>,
    pub default_targets: Vec<String>,

    pub environments: HashMap<String, Environment>,
    pub targets: HashMap<String, Target>,
    pub toolchains: HashMap<String, Toolchain>,

    pub buildfile: config::BuildFile,
}

impl Build {
    pub fn set_default_env(&mut self) {
        let env = self.buildfile
                      .environments
                      .get(&self.buildfile.config.default_env)
                      .cloned().unwrap();

        self.default_env = Some(Environment {
            description: env.description.clone(),
            toolchain: env.toolchain.clone(),
            vars: env.vars.clone(),
        });
    }

    pub fn set_default_targets(&mut self) {
        self.default_targets = self.buildfile.config.default_targets.iter()
                                   .map(|s| expand_string(&s, self.default_env.as_ref(), self.default_env.as_ref(), None).unwrap())
                                   .collect();
    }
    
    pub fn copy_envs(&mut self) {
        for (name, env) in &self.buildfile.environments {
            let mut new_vars = HashMap::new();

            for (k, v) in &env.vars {
                if k == "toolchain" || k == "description" {
                    panic!("The variable name '{}' is not allowed in env.vars!", k);
                }
                new_vars.insert(k.clone(), expand_string(&v, self.default_env.as_ref(), self.default_env.as_ref(), None).unwrap());
            }

            let new_env = Environment {
                description: env.description.clone(),
                toolchain: expand_string(&env.toolchain, self.default_env.as_ref(), self.default_env.as_ref(), None).unwrap(),
                vars: new_vars,
            };

            self.environments.insert(name.clone(), new_env);
        }
    }

    pub fn copy_targets(&mut self) {
        for (name, target) in &self.buildfile.targets {
            let local_env_str = expand_string(&target.env, self.default_env.as_ref(), self.default_env.as_ref(), None).unwrap();
            let local_env = self.environments
                                .get(&local_env_str)
                                .cloned();
            
            let mut new_outputs = HashMap::new();

            for (o_name, output) in &target.outputs {
                let new_output = Output {
                    kind: expand_string(&output.kind, self.default_env.as_ref(), self.default_env.as_ref(), local_env.as_ref()).unwrap(),
                };
                new_outputs.insert(o_name.clone(), new_output);
            }

            let new_target = Target {
                path: expand_string(&target.path, self.default_env.as_ref(), self.default_env.as_ref(), local_env.as_ref()).unwrap(),
                config: expand_string(&target.config, self.default_env.as_ref(), self.default_env.as_ref(), local_env.as_ref()).unwrap(),
                outputs: new_outputs,
                env: local_env_str,
            };

            self.targets.insert(name.clone(), new_target);
        }
    }

    pub fn copy_toolchains(&mut self) {
        for (name, toolchain) in &self.buildfile.toolchains {
            let new_toolchain = Toolchain {
                description: expand_string(&toolchain.description, self.default_env.as_ref(), self.default_env.as_ref(), None).unwrap(),
            };

            self.toolchains.insert(name.clone(), new_toolchain);
        }
    }

    pub fn set_full_paths(&mut self, project_root: &Path) {
        for (name, target) in self.targets.iter_mut() {
            target.path = normalize_path(&target.path, project_root);
            target.config = normalize_path(&target.config, Path::new(""));
        }
    }

    pub fn print_full(&self) {
        println!("--- Config ---");
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
