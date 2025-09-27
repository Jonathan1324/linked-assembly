use std::fs::File;
use std::path::Path;
use std::io::Write;

const DEFAULT_CONFIG: &str = r#"[project]
name = "Project"
version = "1.0.0"
description = "A project!"

[build]
dir = "build"
#default_target = "build"

[tools]
default = "default"
toolchains = "build_toolchains.yaml"
formats = "build_formats.yaml"

[targets.build]
description = "Build the project"
#depends = []
out = "executable"
name = "main"
"#;

const DEFAULT_TOOLCHAIN: &str =
r#"default:
  tools:
    CC:
      when:
        ext:
        - c
        out: object
      deps: ${ OUTPUT }.d
      format: default_gcc_deps

      flags: C_FLAGS
      command: gcc ${ FLAGS } -c ${ INPUT } -MMD -MF ${ OUTPUT }.d -o ${ OUTPUT }
      message: Compiling ${ INPUT }

    CXX:
      when:
        ext:
        - cpp
        out: object
      deps: ${ OUTPUT }.d
      format: default_gcc_deps

      flags: CXX_FLAGS
      command: g++ ${ FLAGS } -c ${ INPUT } -MMD -MF ${ OUTPUT }.d -o ${ OUTPUT }
      message: Compiling ${ INPUT }

    LD:
      when:
        out: executable

      flags: LD_FLAGS
      command: g++ ${ INPUT } ${ FLAGS } -o ${ OUTPUT }
      message: Linking ${ OUTPUT }

    AR:
      when:
        out: static-library

      flags: AR_FLAGS
      command: ar ${ FLAGS } ${ OUTPUT } ${ INPUT }
      message: Creating static library ${ OUTPUT }

  flags:
    C_FLAGS:
      default:
      windows:
      linux:
      macos:

    CXX_FLAGS:
      default:
      windows:
      linux:
      macos:

    LD_FLAGS:
      default:
      windows:
      linux:
      macos:

    AR_FLAGS:
      default:
      - rcs
      windows:
      linux:
      macos:
"#;

const DEFAULT_FORMATS: &str = r#"
default_gcc_deps:
  start:
    ignore_lines: 1
  
  end:
    ignore_lines: 0
    trim:
      if: "\\"
      remove: 1
"#;

pub fn init() -> Result<(), std::io::Error> {
    let path = Path::new("build.toml");
    if path.exists() {
        return Err(std::io::Error::new(
            std::io::ErrorKind::AlreadyExists,
            "build.toml already exists",
        ));
    }

    let mut config = File::create(path)?; // Default
    config.write_all(DEFAULT_CONFIG.as_bytes())?;

    // TODO
    let mut toolchains = File::create("build_toolchains.yaml")?;
    toolchains.write_all(DEFAULT_TOOLCHAIN.as_bytes())?;

    let mut formats = File::create("build_formats.yaml")?;
    formats.write_all(DEFAULT_FORMATS.as_bytes());
    
    Ok(())
}