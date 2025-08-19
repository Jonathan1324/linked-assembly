use walkdir::WalkDir;
use glob::Pattern;
use std::path::Path;
use std::collections::HashMap;
use crate::yaml::target_config;
use crate::yaml::build;

pub fn find_files(base_dir: &Path, filters: Option<&target_config::Filters>) -> Vec<String> {
    let include_patterns: Vec<Pattern> = filters
        .map(|f| f.patterns.clone())
        .unwrap_or_default()
        .iter()
        .map(|p| Pattern::new(&p.replace("\\", "/")).unwrap())
        .collect();

    let exclude_patterns: Vec<Pattern> = filters
        .map(|f| f.exclude.clone()) // kein as_ref nötig
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
                result.push(entry.path().to_string_lossy().to_string());
            }
        }
    }

    result
}


#[derive(Debug, Clone)]
pub struct Files
{
    pub file_paths: Vec<String>,
    pub toolchain: String,
}

#[derive(Debug, Clone)]
pub struct Target<'a> {
    pub targetfile: target_config::TargetFile,

    pub path: String,
    pub files: HashMap<String, Files>,
    pub env: &'a build::Environment,
}

impl Target<'_> {
    pub fn get_files(&mut self, build: &build::Build) {
        for (name, files) in &self.targetfile.files {
            let file_paths = find_files(&Path::new(&self.path), files.filters.as_ref());
            let files = Files {
                file_paths: file_paths,
                toolchain: files.toolchain.clone().unwrap_or(self.env.toolchain.clone()),
            };

            self.files.insert(name.to_string(), files);
        }
    }
}