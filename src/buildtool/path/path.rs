use std::path::{Path, PathBuf};
use std::env;

pub fn normalize_path(path_str: &str, project_root: &Path) -> String {
    let path_str = path_str.replace('\\', "/");
    let path = Path::new(&path_str);
    
    let abs_path = if path.is_absolute() {
        path.to_path_buf()
    } else {
        project_root.join(path)
    };

    let mut components = vec![];
    for comp in abs_path.components() {
        match comp {
            std::path::Component::CurDir => {}
            std::path::Component::ParentDir => { components.pop(); }
            other => components.push(other),
        }
    }

    let normalized = components.iter().collect::<PathBuf>();
    normalized.to_string_lossy().to_string()
}