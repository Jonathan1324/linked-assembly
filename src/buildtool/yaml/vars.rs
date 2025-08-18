use regex::Regex;

pub fn expand_string(input: &str, env: &std::collections::HashMap<String, String>) -> String
{
    let re = Regex::new(r"\$\{([^}]+)\}").unwrap();
    re.replace_all(
        input, |caps: &regex::Captures|
        {
            let key = &caps[1];
            if let Some(stripped) = key.strip_prefix("env.") {
                env.get(stripped).cloned().unwrap_or_else(|| format!("${{{}}}", key))
            } else {
                // Unknown
                format!("${{{}}}", key)
            }
        }
    ).to_string()
}
