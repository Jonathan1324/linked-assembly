use crate::target::files::{self, TargetFile};
use crate::yaml::config;
use crate::yaml::vars::{expand_string, ExpandContext, expand_string_with_vars};
use crate::path::path::normalize_path;
use crate::yaml::target_config;
use std::collections::HashMap;
use std::path::{Path, PathBuf};
use std::sync::Arc;
use std::fs;
use std::io;
use std::process::{Command, ExitStatus};
#[cfg(unix)]
use std::os::unix::fs::PermissionsExt;

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
    pub target: String,
    pub outputs: HashMap<String, Output>,
    pub env: String,

    // Target struct
    pub targetfile: TargetFile,
}

#[derive(Debug, Clone)]
pub struct ToolWhen {
    pub ext: Vec<String>,
    pub kind: String,
}

#[derive(Debug, Clone)]
pub struct Tool {
    pub kind: String,
    pub when: ToolWhen,
    pub command: String,
    pub message: String,
}

#[derive(Debug, Clone)]
pub struct Toolchain {
    pub description: String,

    pub tools: HashMap<String, Tool>,
}

#[derive(Debug, Clone)]
pub struct Build {
    pub default_env: Arc<Environment>,
    pub default_targets: Vec<String>,
    pub project_root: PathBuf,

    pub environments: HashMap<String, Arc<Environment>>,
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

            self.environments.insert(name.clone(), Arc::new(new_env));
        }

        for (name, target) in &self.buildfile.targets {
            let local_env_str = expand_string(&target.env, &ctx).unwrap();
            let local_env = self.environments
                                .get(&local_env_str)
                                .cloned().unwrap();
            let local_ctx = ctx.with_local_env(&local_env);
            
            let mut new_outputs = HashMap::new();

            for (o_name, output) in &target.outputs {
                let kind = expand_string(&output.kind, &local_ctx).unwrap();
                if kind != "source" && kind != "executable" && kind != "object" {
                    panic!("Unknown type of output: {}", kind);
                }
                let new_output = Output {
                    kind: kind,
                };
                new_outputs.insert(o_name.clone(), new_output);
            }

            let target_path = normalize_path(expand_string(&target.path, &local_ctx).unwrap().as_str(), &self.project_root);
            let target_config_file = normalize_path(expand_string(&target.config, &local_ctx).unwrap().as_str(), Path::new(""));

            let config_path = Path::new(&target_path).join(&target_config_file);
            if !config_path.exists() {
                eprintln!("Config file of target {} not found: {:?}", name, config_path);
                std::process::exit(1);
            }
            let config_str = std::fs::read_to_string(&config_path)
                .expect("Failed to read target YAML file");
            let config: target_config::TargetFile = serde_yaml::from_str(&config_str)
                .expect("Failed to parse target YAML");
            let target_env = self.environments.get(&local_env_str).unwrap();

            let mut targetfile = files::TargetFile {
                targetfile: config,
                path: PathBuf::from(target_path.clone()),
                env: target_env.clone(),
                files: HashMap::new(),
                targets: HashMap::new(),
            };
            targetfile.parse(self);

            let new_target = Target {
                path: target_path,
                config: target_config_file,
                target: expand_string(&target.target.clone().unwrap_or("main".to_string()), &ctx).unwrap(), // TODO: currently setting 'main' as default target
                outputs: new_outputs,
                env: local_env_str,
                targetfile: targetfile,
            };

            self.targets.insert(name.clone(), new_target);
        }

        for (name, toolchain) in &self.buildfile.toolchains {
            let mut toolchain_tools = HashMap::new();
            for (tool_name, tool) in &toolchain.tools {
                let mut new_ext = Vec::new();
                for ext in &tool.when.ext {
                    new_ext.push(expand_string(ext, &ctx).unwrap());
                }

                let kind = expand_string(&tool.when.kind, &ctx).unwrap();
                if kind != "source" && kind != "executable" && kind != "object" {
                    panic!("Unknown type of output: {}", kind);
                }
                let new_toolwhen = ToolWhen {
                    ext: new_ext,
                    kind: kind,
                };

                let tool_kind = expand_string(&tool.kind, &ctx).unwrap();
                if tool_kind != "compiler" && tool_kind != "linker" {
                    panic!("Unknown type of output: {}", tool_kind);
                }
                let new_tool = Tool {
                    kind: tool_kind,
                    when: new_toolwhen,
                    command: expand_string(&tool.command, &ctx).unwrap(),
                    message: expand_string(&tool.message, &ctx).unwrap(),
                };

                toolchain_tools.insert(tool_name.clone(), new_tool);
            }
            
            let new_toolchain = Toolchain {
                description: expand_string(&toolchain.description, &ctx).unwrap(),
                tools: toolchain_tools,
            };

            self.toolchains.insert(name.clone(), new_toolchain);
        }
    }

    pub fn resolve_targets(&mut self) {
        for (name, target) in &mut self.targets {
            for (out_name, _out) in &target.outputs {
                if !target.targetfile.targets
                    .get(&target.target)
                    .unwrap()
                    .outputs
                    .contains_key(out_name)
                {
                    panic!("Output {} isn't defined in {} for {}", out_name, target.config, name);
                }
            }
        }
    }

    pub fn parse_target(&self, target: &files::Target, main_target: &Target, input: Option<&String>, combine_outputs: bool) -> Vec<String> {
        let mut dep_outputs = Vec::new();
        for dep in &target.dependencies {
            let dep_files = main_target.targetfile.files.get(dep)
                .expect("Couldn't find dependency of target in files");

            let dep_target = main_target.targetfile.targets.get(&dep_files.target)
                .expect("Couldn't find target of files entry");

            for file_path in &dep_files.file_paths {
                let outputs = self.parse_target(dep_target, main_target, Some(file_path), false);
                dep_outputs.extend(outputs);
            }
        }

        if let Some(input_file) = input {
            let full_input_file = normalize_path(&input_file.as_str(), Path::new(&main_target.path));
            dep_outputs.push(full_input_file.clone());
        }

        let mut vars = HashMap::new();
        if let Some(input_file) = input {
            vars.insert("NAME".to_string(), input_file);
        }

        let mut outputs_of_this_target = Vec::new();
        for (_name, output) in &target.outputs {
            let output_path_str = normalize_path(
                expand_string_with_vars(&output.path, &vars).unwrap().as_str(),
                 Path::new(&main_target.path)
            );
            let output_path = Path::new(&output_path_str);
            if let Some(parent) = output_path.parent() {
                fs::create_dir_all(parent).unwrap();
            }

            if combine_outputs {
                let combined_paths: Vec<&Path> = dep_outputs.iter()
                    .map(|p| Path::new(p))
                    .collect();
                self.run_with_toolchain(combined_paths, output_path, &target.toolchain, &output.kind);
            } else {
                for in_path in &dep_outputs {
                    let in_path = Path::new(in_path);
                    self.run_with_toolchain([in_path].to_vec(), output_path, &target.toolchain, &output.kind);
                }
            }

            outputs_of_this_target.push(output_path_str);
        }

        outputs_of_this_target
    }

    pub fn build(&self) {
        for target_name in &self.default_targets {
            let main_target = self.targets.get(target_name.as_str())
                .expect("Couldn't find target in default targets");
            let targetfile = &main_target.targetfile;

            let target = targetfile.targets.get(&main_target.target)
                .expect("Couldn't find target in targetfile");

            // TODO: set input
            let outs = self.parse_target(target, main_target, None, true);
        }
    }

    pub fn execute_command(&self, command: &str) -> io::Result<ExitStatus> {
        #[cfg(unix)]
        let shell = "sh";
        #[cfg(unix)]
        let shell_arg = "-c";

        #[cfg(windows)]
        let shell = "cmd";
        #[cfg(windows)]
        let shell_arg = "/C";

        let status = Command::new(shell)
            .arg(shell_arg)
            .arg(command)
            .status()?;

        Ok(status)
    }

    pub fn run_with_toolchain(&self, input: Vec<&Path>, output: &Path, toolchain_str: &String, output_kind: &String) {
        let toolchain = self.toolchains.get(toolchain_str)
            .expect("Couldn't find toolchain");

        if input.is_empty() {
            panic!("Haven't gotten any inputs");
        }

        if output_kind == "object" {
            // TODO: not good, expecting every input is kind source when output_kind is object
            let input_kind = "source".to_string();

            let mut found_tool = false;
            for input_path in &input {
                let ext = input_path
                    .extension()
                    .and_then(|s| s.to_str())
                    .unwrap_or("");

                for (tool_name, tool) in &toolchain.tools {
                    let type_match = tool.when.kind == input_kind;
                    let ext_match = tool.when.ext.is_empty() || tool.when.ext.iter().any(|e| e == ext);

                    if type_match && ext_match {
                        let mut vars: HashMap<String, &String> = HashMap::new();
                        let input_path_str = input_path.to_string_lossy().to_string();
                        let output_path_str = output.to_string_lossy().to_string();
                        vars.insert("INPUT".to_string(),&input_path_str);
                        vars.insert("OUTPUT".to_string(),&output_path_str);

                        let command_str = expand_string_with_vars(&tool.command, &vars).unwrap();
                        let message = expand_string_with_vars(&tool.message, &vars).unwrap();

                        // TODO: set variables like ${INPUT}, etc.
                        let status = self.execute_command(&command_str)
                            .expect("Failed to execute command");

                        if !status.success() {
                            panic!("Process returned an error");
                        }

                        println!("{}", message);

                        found_tool = true;
                        break;
                    }
                }
            }
            if !found_tool {
                panic!("Haven't found fitting tool for input_kind");
            }
        } else if output_kind == "executable" {
            // TODO: not good, expecting every input is kind object when output_kind is executable
            let input_kind = "object".to_string();

            let mut found_tool = false;
            let ext = input[0]
                .extension()
                .and_then(|s| s.to_str())
                .unwrap_or("");

            for (tool_name, tool) in &toolchain.tools {
                let type_match = tool.when.kind == input_kind;
                let ext_match = tool.when.ext.is_empty() || tool.when.ext.iter().any(|e| e == ext);

                if type_match && ext_match {
                    let mut vars: HashMap<String, &String> = HashMap::new();
                    let input_path_strings: Vec<String> = input
                        .iter()
                        .map(|p| p.to_string_lossy().to_string())
                        .collect();
                    let input_path_str = input_path_strings.join(" ");

                    let output_path_str = output.to_string_lossy().to_string();
                    vars.insert("INPUT".to_string(),&input_path_str);
                    vars.insert("OUTPUT".to_string(),&output_path_str);

                    let command_str = expand_string_with_vars(&tool.command, &vars).unwrap();
                    let message = expand_string_with_vars(&tool.message, &vars).unwrap();

                    let status = self.execute_command(&command_str)
                        .expect("Failed to execute command");

                    if !status.success() {
                        panic!("Process returned an error");
                    }

                    println!("{}", message);

                    found_tool = true;
                    break;
                }
            }
            if !found_tool {
                panic!("Haven't found fitting tool for input_kind");
            }
            // Unix: chmod +x
            #[cfg(unix)]
            {
                let mut perms = fs::metadata(output).unwrap().permissions();
                perms.set_mode(0o755);
                fs::set_permissions(output, perms).unwrap();
            }
        } else {
            panic!("Unknown type of target");
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
            for (tool_name, tool) in &tc.tools {
                println!("  {}:", tool_name);
                println!("    command: {}", tool.command);
                println!("    type: {}", tool.kind);
                println!("    when:");
                println!("      type: {}", tool.when.kind);
                println!("      ext:");
                for ext in &tool.when.ext {
                    println!("      - {}", ext);
                }
            }
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
            println!("  config: {}", target.config);
            println!("  target: {}", target.target);
            println!("  env: {}", target.env);

            if target.outputs.is_empty() {
                println!("  outputs: (none)");
            } else {
                println!("  outputs:");
                for (name, out) in &target.outputs {
                    println!("    {}: {}", name, out.kind);
                }
            }

            println!("  file:");
            target.targetfile.print_full();
        }
    }
}
