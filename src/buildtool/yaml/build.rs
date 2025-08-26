use crate::target::files::{self, TargetFile};
use crate::yaml::{build, config};
use crate::yaml::vars::{expand_string, ExpandContext, expand_string_with_vars};
use crate::path::path::normalize_path;
use crate::yaml::target_config;
use crate::cache::cache;
use std::collections::{HashMap, HashSet};
use std::path::{Path, PathBuf};
use std::sync::Arc;
use std::fs;
use std::io::{self, BufRead};
use std::process::{Command, ExitStatus};
#[cfg(unix)]
use std::os::unix::fs::PermissionsExt;

#[derive(Debug, Clone)]
pub struct Environment {
    pub description: String,

    pub toolchain: String,

    pub build_dir: String,

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
pub struct Trim {
    pub condition: String,
    pub remove: u32,
}

#[derive(Debug, Clone)]
pub struct FormatRule {
    pub ignore_lines: u32,
    pub trim: Option<Trim>
}

#[derive(Debug, Clone)]
pub struct Format {
    pub start: Option<FormatRule>,
    pub end: Option<FormatRule>,
}

#[derive(Debug, Clone)]
pub struct ToolWhen {
    pub ext: Option<Vec<String>>,
    pub kind: String,
    pub out: String,
}

#[derive(Debug, Clone)]
pub struct Tool {
    pub kind: String,
    pub when: ToolWhen,
    pub dep_path: Option<String>,
    pub dep_format: String,
    pub combine_inputs: bool,
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
    pub formats: HashMap<String, Format>,

    pub buildfile: config::BuildFile,

    pub os: String,
}

impl Build {

    pub fn copy_from_file(&mut self) {
        let  mut ctx = ExpandContext::new(&self.os);
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

            for (k, values) in &env.specific {
                if k == "toolchain" || k == "description" {
                    panic!("The variable name '{}' is not allowed in env.vars!", k);
                }
                for value in values {
                    let when = &value.when;

                    let mut broken = false;
                    if let Some(os) = &when.os {
                        if *os != self.os && !broken {
                            broken = true;
                        }
                    }

                    if !broken {
                        new_vars.insert(k.clone(), expand_string(&value.then, &ctx).unwrap());
                        break;
                    }
                }
            }

            let build_dir = normalize_path(expand_string(&env.build_dir, &ctx).unwrap().as_str(), &self.project_root);

            let new_env = Environment {
                description: env.description.clone(),
                toolchain: expand_string(&env.toolchain, &ctx).unwrap(),
                build_dir: build_dir,
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
                let mut new_ext = None;
                if let Some(ext_vec) = &tool.when.ext {
                    if tool.combine_inputs {
                        panic!("Can't combine inputs and have when.ext");
                    }
                    let mut new_ext_vec: Vec<String> = Vec::new();
                    for ext in ext_vec {
                        new_ext_vec.push(expand_string(ext, &ctx).unwrap());
                    }
                    new_ext = Some(new_ext_vec);
                }

                let kind = expand_string(&tool.when.kind, &ctx).unwrap();
                if kind != "source" && kind != "executable" && kind != "object" {
                    panic!("Unknown type of output: {}", kind);
                }
                let out = expand_string(&tool.when.out, &ctx).unwrap();
                if out != "executable" && out != "object" {
                    panic!("Unknown type of output: {}", out);
                }
                let new_toolwhen = ToolWhen {
                    ext: new_ext,
                    kind: kind,
                    out: out,
                };

                let mut dep_path = None;
                if let Some(dep) = &tool.dep_path {
                    dep_path = Some(expand_string(dep, &ctx).unwrap());
                }

                let mut dep_format = "$$".to_string();
                if let Some(format) = &tool.dep_format {
                    dep_format = expand_string(format, &ctx).unwrap();
                }

                let tool_kind = expand_string(&tool.kind, &ctx).unwrap();
                if tool_kind != "compiler" && tool_kind != "linker" {
                    panic!("Unknown type of output: {}", tool_kind);
                }
                let new_tool = Tool {
                    kind: tool_kind,
                    when: new_toolwhen,
                    combine_inputs: tool.combine_inputs,
                    dep_path: dep_path,
                    dep_format: dep_format,
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

        for (name, format) in &self.buildfile.formats {
            let mut new_start = None;
            if let Some(start) = &format.start {
                let mut new_trim = None;
                if let Some(trim) = &start.trim {
                    new_trim = Some(Trim {
                        condition: expand_string(&trim.condition, &ctx).unwrap(),
                        remove: trim.remove
                    });
                }

                new_start = Some(FormatRule {
                    ignore_lines: start.ignore_lines,
                    trim: new_trim,
                });
            }

            let mut new_end = None;
            if let Some(end) = &format.end {
                let mut new_trim = None;
                if let Some(trim) = &end.trim {
                    new_trim = Some(Trim {
                        condition: expand_string(&trim.condition, &ctx).unwrap(),
                        remove: trim.remove
                    });
                }

                new_end = Some(FormatRule {
                    ignore_lines: end.ignore_lines,
                    trim: new_trim,
                });
            }

            let new_format = Format {
                start: new_start,
                end: new_end,
            };

            self.formats.insert(name.clone(), new_format);
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

    pub fn parse_target(&self, target: &files::Target, main_target: &Target, input: Option<&String>, cache: &cache::CacheBuffer) -> (Vec<String>, bool) {
        let mut dep_outputs = Vec::new();
        let mut any_rebuilt = false;

        for dep in &target.dependencies {
            let dep_files = main_target.targetfile.files.get(dep)
                .expect("Couldn't find dependency of target in files");

            let dep_target = main_target.targetfile.targets.get(&dep_files.target)
                .expect("Couldn't find target of files entry");

            for file_path in &dep_files.file_paths {
                let (outputs, rebuilt) = self.parse_target(dep_target, main_target, Some(file_path), cache);
                dep_outputs.extend(outputs);
                if rebuilt {
                    any_rebuilt = true;
                }
            }
        }

        if let Some(input_file) = input {
            let full_input_file = normalize_path(&input_file.as_str(), Path::new(&main_target.path));
            dep_outputs.push(full_input_file.clone());
        }

        let mut vars = HashMap::new();
        if let Some(input_file) = input {
            let path = Path::new(&input_file);

            vars.insert("INPUT".to_string(), path.to_string_lossy().to_string());

            if let Some(stem) = path.file_stem().and_then(|s| s.to_str()) {
                vars.insert("NAME".to_string(), stem.to_string());
            }

            if let Some(ext) = path.extension().and_then(|s| s.to_str()) {
                vars.insert("EXT".to_string(), ext.to_string());
            }

            if let Some(parent) = path.parent().and_then(|p| p.to_str()) {
                let path = parent.to_string();
                if path.is_empty() {
                    vars.insert("PATH".to_string(), ".".to_string());
                } else {
                    vars.insert("PATH".to_string(), parent.to_string());
                }
            }
        }

        let mut outputs_of_this_target = Vec::new();
        for (_name, output) in &target.outputs {
            let mut build_path = PathBuf::from(&main_target.targetfile.env.build_dir);
            // TODO: set better name
            build_path.push("out");

            let output_path_str = normalize_path(
                expand_string_with_vars(&output.path, &vars).unwrap().as_str(),
                 &build_path
            );

            let output_path = Path::new(&output_path_str);
            if let Some(parent) = output_path.parent() {
                fs::create_dir_all(parent).unwrap();
            }

            let combined_paths: Vec<&Path> = dep_outputs.iter()
                    .map(|p| Path::new(p))
                    .collect();

            let rebuilt = self.run_with_toolchain(
                cache,
                combined_paths,
                output_path,
                &target.toolchain,
                &output.kind,
                &main_target.targetfile.env.build_dir,
                any_rebuilt
            );

            if rebuilt {
                any_rebuilt = true;
            }

            outputs_of_this_target.push(output_path_str);
        }

        (outputs_of_this_target, any_rebuilt)
    }

    pub fn parse_deps(&self, file: &fs::File, format: &Format) -> io::Result<Vec<String>> {
        let reader = io::BufReader::new(file);
        let mut lines: Vec<String> = reader.lines().collect::<Result<_, _>>()?;

        let start_skip = format.start.as_ref().map(|r| r.ignore_lines as usize).unwrap_or(0);
        let end_skip   = format.end  .as_ref().map(|r| r.ignore_lines as usize).unwrap_or(0);

        if lines.len() < start_skip + end_skip {
            return Ok(Vec::new());
        }

        let range_end = lines.len() - end_skip;
        let mut deps = Vec::new();

        for mut line in lines.drain(start_skip..range_end) {
            line = line.trim().to_string();

            if let Some(start) = &format.start {
                if let Some(trim) = &start.trim {
                    if line.starts_with(&trim.condition) {
                        let n = trim.remove as usize;
                        if line.len() >= n {
                            line.drain(0..n);
                        } else {
                            line.clear();
                        }
                    }
                }
            }

            if let Some(end) = &format.end {
                if let Some(trim) = &end.trim {
                    if line.ends_with(&trim.condition) {
                        let n = trim.remove as usize;
                        if line.len() >= n {
                            let new_len = line.len() - n;
                            line.truncate(new_len);
                        } else {
                            line.clear();
                        }
                    }
                }
            }

            let s = line.trim();
            if !s.is_empty() {
                deps.push(s.to_string());
            }
        }

        Ok(deps)
    }

    pub fn build(&self) {
        let mut cache_dir = PathBuf::from(&self.default_env.build_dir);
        cache_dir.push(".cache");
        if !cache_dir.exists() {
            fs::create_dir_all(&cache_dir).unwrap();
        }
        let cache_file = cache_dir.join("cache.dat");

        let cache = cache::CacheBuffer::parse_file(&cache_file).unwrap_or_else(|| {
            panic!("Could not create cache buffer");
        });

        for target_name in &self.default_targets {
            let main_target = self.targets.get(target_name.as_str())
                .expect("Couldn't find target in default targets");
            let targetfile = &main_target.targetfile;

            let target = targetfile.targets.get(&main_target.target)
                .expect("Couldn't find target in targetfile");

            // TODO: set input
            let (outs, rebuilt) = self.parse_target(target, main_target, None, &cache);
        }

        cache.write_file(&cache_file);
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

    pub fn run_with_toolchain(
        &self,
        cache: &cache::CacheBuffer,
        input: Vec<&Path>,
        output: &Path,
        toolchain_str: &String,
        output_kind: &String,
        build_dir: &String,
        force_rebuild: bool
    ) -> bool {
        let toolchain = self.toolchains.get(toolchain_str)
            .expect("Couldn't find toolchain");

        let mut cache_dir = PathBuf::from(&self.default_env.build_dir);
        cache_dir.push(".cache");
        if !cache_dir.exists() {
            fs::create_dir_all(&cache_dir).unwrap();
        }

        let mut input_files: Vec<PathBuf> = Vec::new();
        // TODO: better way of getting source files
        if output_kind == "object" {
            input_files = input.iter().map(|p| (*p).to_path_buf()).collect();
        }

        let tools: HashMap<&String, &Tool> = toolchain.tools
            .iter()
            .filter(|(_, tool) | (tool.when.out == *output_kind))
            .collect();

        // TODO: not always set true
        let create_name_map = true;

        let mut file_matches: HashMap<&Path, &Tool> = HashMap::new();

        let mut any_rebuilt = false;

        let mut combine_matched = false;
        let mut per_file_matches: HashSet<&Path> = HashSet::new();

        for (_name, tool) in tools {
            if tool.combine_inputs {
                let file_kind = "object"; // TODO: not a nice way

                let kind_ok = tool.when.kind == file_kind;

                if kind_ok {
                    combine_matched = true;
                    if let Some(prev_tool) = file_matches.get(output) {
                        panic!("Multiple tools work for {}", output.display())
                    }
                    file_matches.insert(output, tool);

                    if let Some(dep_path) = &tool.dep_path {
                        panic!("Can't combine inputs and have deps");
                    }

                    if cache::check_built(&cache_dir, &input_files, &output.to_string_lossy(), cache) && !force_rebuild {
                        break;
                    }
                    any_rebuilt = true;

                    let mut vars = HashMap::new();
                    let input_path_strings: Vec<String> = input
                        .iter()
                        .map(|p| p.to_string_lossy().to_string())
                        .collect();
                    let input_path_str = input_path_strings.join(" ");
                    vars.insert("INPUT".to_string(),input_path_str.clone());
                    vars.insert("OUTPUT".to_string(),output.to_string_lossy().to_string());
                    if let Some(stem) = output.file_stem().and_then(|s| s.to_str()) {
                        vars.insert("ONAME".to_string(), stem.to_string());
                    }
                    if let Some(ext) = output.extension().and_then(|s| s.to_str()) {
                        vars.insert("OEXT".to_string(), ext.to_string());
                    }
                    if let Some(parent) = output.parent().and_then(|p| p.to_str()) {
                        let path = parent.to_string();
                        if path.is_empty() {
                            vars.insert("OPATH".to_string(), ".".to_string());
                        } else {
                            vars.insert("OPATH".to_string(), parent.to_string());
                        }
                    }

                    let command_str = expand_string_with_vars(&tool.command, &vars).unwrap();
                    let message = expand_string_with_vars(&tool.message, &vars).unwrap();

                    let status = self.execute_command(&command_str)
                        .expect("Failed to execute command");

                    if !status.success() {
                        panic!("Process returned an error");
                    }

                    if !message.is_empty() {
                        println!("{}", message);
                    }

                    cache::write_built(&cache_dir, &input_files, &output.to_string_lossy(), create_name_map, cache);

                    break;
                }
            } else {
                for file in &input {
                    let ext = file.extension().and_then(|s| s.to_str()).unwrap_or("");
                    let file_kind = "source"; // TODO: not a nice way

                    let ext_ok = tool.when.ext.as_ref()
                        .map_or(true, |exts| exts.iter().any(|e| e == ext));
                    let kind_ok = tool.when.kind == file_kind;

                    if ext_ok && kind_ok {
                        per_file_matches.insert(file);

                        if let Some(prev_tool) = file_matches.get(file) {
                            panic!("Multiple tools work for {}", file.display())
                        }
                        file_matches.insert(file, tool);

                        let mut vars = HashMap::new();
                        vars.insert("INPUT".to_string(),file.to_string_lossy().to_string());
                        if let Some(stem) = file.file_stem().and_then(|s| s.to_str()) {
                            vars.insert("NAME".to_string(), stem.to_string());
                        }
                        if let Some(ext) = file.extension().and_then(|s| s.to_str()) {
                            vars.insert("EXT".to_string(), ext.to_string());
                        }
                        if let Some(parent) = file.parent().and_then(|p| p.to_str()) {
                            let path = parent.to_string();
                            if path.is_empty() {
                                vars.insert("PATH".to_string(), ".".to_string());
                            } else {
                                vars.insert("PATH".to_string(), parent.to_string());
                            }
                        }
                        vars.insert("OUTPUT".to_string(),output.to_string_lossy().to_string());
                        if let Some(stem) = output.file_stem().and_then(|s| s.to_str()) {
                            vars.insert("ONAME".to_string(), stem.to_string());
                        }
                        if let Some(ext) = output.extension().and_then(|s| s.to_str()) {
                            vars.insert("OEXT".to_string(), ext.to_string());
                        }
                        if let Some(parent) = output.parent().and_then(|p| p.to_str()) {
                            let path = parent.to_string();
                            if path.is_empty() {
                                vars.insert("OPATH".to_string(), ".".to_string());
                            } else {
                                vars.insert("OPATH".to_string(), parent.to_string());
                            }
                        }

                        let mut set_deps = false;
                        if let Some(dep_path) = &tool.dep_path {
                            let path = expand_string_with_vars(&dep_path, &vars).unwrap();
                            if !Path::new(&path).exists() {
                                eprintln!("Warning: Dependency file '{}' does not exist.", path);
                            } else {
                                let file = fs::File::open(&path)
                                    .unwrap_or_else(|err| panic!("Couldn't open dependency file '{}': {}", path, err));

                                let format_str = expand_string_with_vars(&tool.dep_format, &vars).unwrap();
                                let format = self.formats.get(&format_str)
                                    .unwrap_or_else(|| panic!("Couldn't find format '{}'", format_str));

                                let deps = self.parse_deps(&file, &format)
                                    .unwrap_or_else(|err| panic!("Failed to parse dependencies from '{}': {}", path, err));

                                for dep_str in &deps {
                                    let dep = PathBuf::from(dep_str);
                                    if !dep.exists() {
                                        eprintln!("Warning: Dependency '{}' does not exist.", dep.display());
                                    } else {
                                        input_files.push(dep);
                                    }
                                }

                                set_deps = true;
                            }
                        }

                        if cache::check_built(&cache_dir, &input_files, &output.to_string_lossy(), cache) && !force_rebuild {
                            continue;
                        }
                        any_rebuilt = true;

                        let command_str = expand_string_with_vars(&tool.command, &vars).unwrap();
                        let message = expand_string_with_vars(&tool.message, &vars).unwrap();

                        let status = self.execute_command(&command_str)
                            .expect("Failed to execute command");

                        if !status.success() {
                            panic!("Process returned an error");
                        }

                        if !message.is_empty() {
                            println!("{}", message);
                        }

                        if !set_deps {
                            if let Some(dep_path) = &tool.dep_path {
                                let path = expand_string_with_vars(&dep_path, &vars).unwrap();
                                if Path::new(&path).exists() {
                                    let file = fs::File::open(&path)
                                        .unwrap_or_else(|err| panic!("Couldn't open dependency file '{}': {}", path, err));

                                    let format_str = expand_string_with_vars(&tool.dep_format, &vars).unwrap();
                                    let format = self.formats.get(&format_str)
                                        .unwrap_or_else(|| panic!("Couldn't find format '{}'", format_str));

                                    let deps = self.parse_deps(&file, &format)
                                        .unwrap_or_else(|err| panic!("Failed to parse dependencies from '{}': {}", path, err));

                                    for dep_str in &deps {
                                        let dep = PathBuf::from(dep_str);
                                        if dep.exists() {
                                            input_files.push(dep);
                                        }
                                    }

                                    set_deps = true;
                                }
                            }
                        }

                        cache::write_built(&cache_dir, &input_files, &output.to_string_lossy(), create_name_map, cache);
                    }
                }
            }
        }

        if !combine_matched {
            for file in &input {
                if !per_file_matches.contains(file) {
                    panic!("No tool found for output {}", output.display());
                }
            }
        }

        if output_kind == "executable" {
            // Unix: chmod +x
            #[cfg(unix)]
            {
                let mut perms = fs::metadata(output).unwrap().permissions();
                perms.set_mode(0o755);
                fs::set_permissions(output, perms).unwrap();
            }
        }

        return any_rebuilt;
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

        println!("\n--- Formats ---");
        for (name, format) in &self.formats {
            println!("Format: {}", name);
            if let Some(start) = &format.start {
                println!("  start:");
                println!("    ignore_lines: {}", start.ignore_lines);
                if let Some(trim) = &start.trim {
                    println!("    trim:");
                    println!("      if: '{}'", trim.condition);
                    println!("      remove: {}", trim.remove);
                }
            }
            if let Some(end) = &format.end {
                println!("  end:");
                println!("    ignore_lines: {}", end.ignore_lines);
                if let Some(trim) = &end.trim {
                    println!("    trim:");
                    println!("      if: '{}'", trim.condition);
                    println!("      remove: {}", trim.remove);
                }
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
                if let Some(dep_path) = &tool.dep_path {
                    println!("    dep_path: {}", dep_path);
                }
                println!("    dep_format: '{}'", tool.dep_format);
                println!("    combine_inputs: {}", tool.combine_inputs);
                println!("    when:");
                println!("      type: {}", tool.when.kind);
                if let Some(ext_vec) = &tool.when.ext {
                    println!("      ext:");
                    for ext in ext_vec {
                        println!("      - {}", ext);
                    }
                }
            }
        }

        println!("\n--- Environments ---");
        for (name, env) in &self.environments {
            println!("Environment: {}", name);
            println!("  description: {}", env.description);
            println!("  toolchain: {}", env.toolchain);
            println!("  build_dir: {}", env.build_dir);
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
