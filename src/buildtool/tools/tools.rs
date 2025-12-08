use serde::Deserialize;
use std::collections::HashMap;

#[derive(Clone, Debug, Deserialize)]
pub struct Toolchain {
    pub tools: HashMap<String, Tool>,
    pub flags: HashMap<String, Flags>,
    pub flagsets: HashMap<String, Vec<String>>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Flags {
    pub global: FlagsSpecific,
    pub platforms: Option<FlagsPlatform>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct FlagsPlatform {
    pub windows: Option<FlagsSpecific>,
    pub linux: Option<FlagsSpecific>,
    pub macos: Option<FlagsSpecific>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct FlagsSpecific {
    pub always: Vec<String>,
    pub modes: HashMap<String, Vec<String>>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Tool {
    pub when: When,

    pub deps: Option<String>,
    pub format: Option<String>,

    pub flags: Option<String>,

    pub command: Vec<String>,
    pub message: Option<String>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct When {
    pub ext: Option<Vec<String>>,
    pub out: crate::config::OutputKind,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Format {
    pub split: Option<String>,
    pub start: Option<FormatRule>,
    pub end: Option<FormatRule>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct FormatRule {
    #[serde(default)]
    pub ignore_lines: u32,
    pub trim: Option<Trim>,
}

#[derive(Clone, Debug, Deserialize)]
pub struct Trim {
    #[serde(rename = "if")]
    pub condition: String,
    pub remove: u32,
}

pub fn print_toolchains(toolchains: &HashMap<String, Toolchain>) {
	for (toolchain_name, toolchain) in toolchains {
        println!("{}: ", toolchain_name);
        println!(" Tools: ");
        for (tool_name, tool) in &toolchain.tools {
            println!("    {}: ", tool_name);
            println!("      Command:");
            for cmd in &tool.command {
                println!("      - {}", cmd);
            }
            if let Some(deps) = &tool.deps {
                println!("      Deps: {}", deps);
            }
            if let Some(format) = &tool.format {
                println!("      Format: {}", format);
            }
            if let Some(flags) = &tool.flags {
                println!("      Flags: {}", flags);
            }
            if let Some(message) = &tool.message {
                println!("      Message: {}", message);
            }
            println!("      when:");
            println!("        out: {:?}", tool.when.out);
            if let Some(exts) = &tool.when.ext {
                if !exts.is_empty() {
                    println!("        ext:");
                    for ext in exts {
                        println!("        - {}", ext);
                    }
                }
            }
        }

        println!(" Flags: ");
        for (flag_name, flag) in &toolchain.flags {
            println!("    {}:", flag_name);

            println!("      global:");
            println!("        always:");
            for flagset in &flag.global.always {
                println!("        - {}", flagset);
            }
            println!("        modes:");
            for (mode_name, mode) in &flag.global.modes {
                println!("          {}:", mode_name);
                for flags in mode {
                    println!("          - {}", flags);
                }
            }

            if let Some(platforms) = &flag.platforms {
                println!("      platforms:");

                if let Some(windows) = &platforms.windows {
                    println!("        windows:");

                    println!("          always:");
                    for flagset in &windows.always {
                        println!("          - {}", flagset);
                    }
                    println!("          modes:");
                    for (mode_name, mode) in &windows.modes {
                        println!("          {}:", mode_name);
                        for flags in mode {
                            println!("            - {}", flags);
                        }
                    }
                }

                if let Some(linux) = &platforms.linux {
                    println!("        linux:");

                    println!("          always:");
                    for flagset in &linux.always {
                        println!("          - {}", flagset);
                    }
                    println!("          modes:");
                    for (mode_name, mode) in &linux.modes {
                        println!("          {}:", mode_name);
                        for flags in mode {
                            println!("            - {}", flags);
                        }
                    }
                }

                if let Some(macos) = &platforms.macos {
                    println!("        macos:");

                    println!("          always:");
                    for flagset in &macos.always {
                        println!("          - {}", flagset);
                    }
                    println!("          modes:");
                    for (mode_name, mode) in &macos.modes {
                        println!("          {}:", mode_name);
                        for flags in mode {
                            println!("            - {}", flags);
                        }
                    }
                }
            }
        }

        println!(" FlagSets: ");
        for (flagset_name, flagset) in &toolchain.flagsets {
            println!("    {}: ", flagset_name);
            for flag in flagset {
                println!("      - '{}'", flag);
            }
        }
    }
}
