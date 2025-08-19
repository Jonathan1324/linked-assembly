use std::collections::HashMap;
use crate::yaml::build;

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

pub fn expand_string(
    input: &str,
    default_env: Option<&build::Environment>,
    env: Option<&build::Environment>,
    local_env: Option<&build::Environment>,
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

            if push_var_from_env(key, "env.", env, &mut result)? {}
            else if push_var_from_env(key, "$env.", local_env, &mut result)? {}
            else if push_var_from_env(key, "!env.", default_env, &mut result)? {}
            else {
                return Err(ExpandError::UnknownVariable(key.to_string()));
            }
        } else {
            result.push(c);
        }
    }

    Ok(result)
}
