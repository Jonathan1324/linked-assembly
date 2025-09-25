use walkdir::WalkDir;
use glob::Pattern;
use std::path::Path;
use std::collections::HashMap;
use std::path::PathBuf;
use std::sync::Arc;
use crate::yaml::target_config;
use crate::yaml::build;
use crate::yaml::vars::{expand_string, ExpandContext};

pub fn find_files(base_dir: &Path, filters: Option<&target_config::Filters>) -> Vec<String> {
    let include_patterns: Vec<Pattern> = filters
        .map(|f| f.patterns.clone())
        .unwrap_or_default()
        .iter()
        .map(|p| Pattern::new(&p.replace("\\", "/")).unwrap())
        .collect();

    let exclude_patterns: Vec<Pattern> = filters
        .map(|f| f.exclude.clone())
        .unwrap_or_default()
        .iter()
        .map(|p| Pattern::new(&p.replace("\\", "/")).unwrap())
        .collect();

    let mut result = Vec::new();

    for entry in WalkDir::new(base_dir).into_iter().filter_map(Result::ok) {
        if entry.file_type().is_file() {
            let file_name = entry.path().file_name().unwrap().to_string_lossy().replace("\\", "/");

            let included = include_patterns.is_empty() || include_patterns.iter().any(|p| p.matches(&file_name));
            let excluded = !exclude_patterns.is_empty() && exclude_patterns.iter().any(|p| p.matches(&file_name));

            if included && !excluded {
                if let Ok(rel_path) = entry.path().strip_prefix(base_dir) {
                    result.push(rel_path.to_string_lossy().replace("\\", "/").to_string());
                }
            }
        }
    }

    result
}


#[derive(Debug, Clone)]
pub struct Files {
    pub file_paths: Vec<String>,
    pub target: String,
    pub at_once: bool,
}

#[derive(Debug, Clone)]
pub struct Output {
    pub path: String,
    pub kind: String,
}

#[derive(Debug, Clone)]
pub struct Target {
    pub toolchain: String,
    pub dependencies: Vec<String>,
    pub outputs: HashMap<String, Output>,
}

#[derive(Debug, Clone)]
pub struct TargetFile {
    pub targetfile: target_config::TargetFile,

    pub path: PathBuf,
    pub env: Arc<build::Environment>,

    pub files: HashMap<String, Files>,
    pub targets: HashMap<String, Target>,
}

impl TargetFile {
    pub fn parse(&mut self, build: &build::Build) {
        let  mut ctx = ExpandContext::new(&build.os);
        ctx.default_env(&build.default_env)
           .env(&self.env)
           .project_root(&build.project_root)
           .path(&self.path);

        for (name, files) in &self.targetfile.files {
            let file_paths = find_files(&Path::new(&self.path), files.filters.as_ref());
            let new_files = Files {
                file_paths: file_paths,
                target: expand_string(&files.target, &ctx).unwrap(),
                at_once: files.at_once
            };

            self.files.insert(name.to_string(), new_files);
        }

        for (name, target) in &self.targetfile.targets {
            let mut dependencies = Vec::new();
            for v in &target.depends {
                dependencies.push(expand_string(&v, &ctx).unwrap());
            }

            let mut outputs = HashMap::new();
            for (name, output) in &target.outputs {
                let new_output = Output {
                    path: expand_string(&output.path, &ctx).unwrap(),
                    kind: expand_string(&output.kind, &ctx).unwrap(),
                };
                if new_output.kind != "source" && new_output.kind != "executable" && new_output.kind != "static-library" && new_output.kind != "object" {
                    panic!("Unknown type of output: {}", new_output.kind);
                }
                outputs.insert(name.clone(), new_output);
            }

            let new_target = Target {
                toolchain: expand_string(&target.toolchain.clone().unwrap_or(self.env.toolchain.clone()), &ctx).unwrap(),
                dependencies: dependencies,
                outputs: outputs,
            };

            self.targets.insert(name.to_string(), new_target);
        }
    }

    pub fn print_full(&self) {
        println!("    path: {}", self.path.to_string_lossy().to_string());

        println!("    files:");
        for (name, file) in &self.files {
            println!("      {}:", name);
            println!("        target: {}", file.target);
            println!("        at_once: {}", file.at_once);
            println!("        files: ");
            for file_path in &file.file_paths {
                println!("      {}", file_path);
            }
        }

        println!("    targets:");
        for (name, target) in &self.targets {
            println!("      {}", name);
            println!("        toolchain: {}", target.toolchain);
            println!("        dependencies:");
            for dep in &target.dependencies {
                println!("        - {}", dep);
            }
            println!("        outputs:");
            for (name, output) in &target.outputs {
                println!("          {}:", name);
                println!("            path: {}", output.path);
                println!("            type: {}", output.kind);
            }
        }
    }
}
