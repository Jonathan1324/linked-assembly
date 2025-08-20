use crate::yaml::build;
use std::{collections::HashMap, path::Path};

#[allow(dead_code)]
#[derive(Debug)]
pub enum ExpandError {
    MissingVariable(String),
    UnknownVariable(String),
}

fn push_var_from_env(
    key: &str,
    prefix: &str,
    maybe_env: Option<&build::Environment>,
    result: &mut String,
) -> Result<bool, ExpandError> {
    if let Some(stripped) = key.strip_prefix(prefix) {
        let env = maybe_env.ok_or_else(|| ExpandError::MissingVariable(key.to_string()))?;

        match stripped {
            "toolchain" => {
                result.push_str(&env.toolchain);
                return Ok(true);
            }
            "description" => {
                result.push_str(&env.description);
                return Ok(true);
            }
            _ => {}
        }

        if let Some(val) = env.vars.get(stripped) {
            result.push_str(val);
            Ok(true)
        } else {
            Err(ExpandError::UnknownVariable(key.to_string()))
        }
    } else {
        Ok(false)
    }
}

#[derive(Debug, Clone)]
pub struct ExpandContext<'a> {
    pub default_env: Option<&'a build::Environment>,
    pub env: Option<&'a build::Environment>,
    pub local_env: Option<&'a build::Environment>,
    pub path: Option<&'a Path>,
    pub project_root: Option<&'a Path>,
}

impl<'a> ExpandContext<'a> {
    pub fn new() -> Self {
        Self {
            default_env: None,
            env: None,
            local_env: None,
            path: None,
            project_root: None,
        }
    }

    pub fn default_env(&mut self, env: &'a build::Environment) -> &mut Self {
        self.default_env = Some(env);
        self
    }

    pub fn env(&mut self, env: &'a build::Environment) -> &mut Self {
        self.env = Some(env);
        self
    }

    pub fn project_root(&mut self, project_root: &'a Path) -> &mut Self {
        self.project_root = Some(project_root);
        self
    }

    pub fn path(&mut self, path: &'a Path) -> &mut Self {
        self.path = Some(path);
        self
    }

    pub fn with_local_env(&self, env: &'a build::Environment) -> Self {
        let mut clone = self.clone();
        clone.local_env = Some(env);
        clone
    }
}

pub fn expand_string(
    input: &str,
    ctx: &ExpandContext,
) -> Result<String, ExpandError> {
    let mut result = String::new();
    let mut chars = input.chars().peekable();

    while let Some(c) = chars.next() {
        if c == '$' && chars.peek() == Some(&'{') {
            chars.next();
            let mut key = String::new();
            while let Some(&ch) = chars.peek() {
                if ch == '}' {
                    chars.next(); 
                    break;
                }
                key.push(ch);
                chars.next();
            }

            let key = key.trim();

            if push_var_from_env(key, "env.", ctx.env, &mut result)? {}
            else if push_var_from_env(key, "$env.", ctx.local_env, &mut result)? {}
            else if push_var_from_env(key, "!env.", ctx.default_env, &mut result)? {}
            else if let Some(stripped) = key.strip_prefix("path.") {
                match stripped {
                    "current" => result.push_str(&ctx.path.unwrap().to_string_lossy()),
                    "project_root" => result.push_str(&ctx.project_root.unwrap().to_string_lossy()),

                    _ => return Err(ExpandError::UnknownVariable(key.to_string())),
                }
            }
            else if key == "NAME" {
                result.push_str("${NAME}");
            }
            else if key == "INPUT" {
                result.push_str("${INPUT}");
            }
            else if key == "OUTPUT" {
                result.push_str("${OUTPUT}");
            }
            else {
                return Err(ExpandError::UnknownVariable(key.to_string()));
            }
        } else {
            result.push(c);
        }
    }

    Ok(result)
}

pub fn expand_string_with_vars(
    input: &str,
    vars: &HashMap<String, &String>
) -> Result<String, ExpandError> {
    let mut result = String::new();
    let mut chars = input.chars().peekable();

    while let Some(c) = chars.next() {
        if c == '$' && chars.peek() == Some(&'{') {
            chars.next();
            let mut key = String::new();
            while let Some(&ch) = chars.peek() {
                if ch == '}' {
                    chars.next(); 
                    break;
                }
                key.push(ch);
                chars.next();
            }

            let key = key.trim();

            if let Some(value) = vars.get(key) {
                result.push_str(value);
            } else {
                return Err(ExpandError::UnknownVariable(key.to_string()))
            }
        } else {
            result.push(c);
        }
    }

    Ok(result)
}
