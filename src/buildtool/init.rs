use std::fs::{self, File};
use std::path::Path;
use std::io::Write;

const DEFAULT_CONFIG: &str =
r#"[project]
name = "Project"
version = "1.0.0"
description = "A project!"

[build]
dir = "build/debug"
default_target = "build"

[tools]
default_toolchain = "default"
default_mode = "debug"
toolchains = ["build-config/build_toolchains.yaml"]
formats = ["build-config/build_formats.yaml"]

[flags.release]
build_dir = "build/release"
build_mode = "release"

[targets.run]
before = ["build"]
run = "execute ${BUILD_DIR}/main"

[targets.clean]
run = "delete ${BUILD_DIR}"

[targets.build]
description = "Build the project"
depends = ["compile_c_cpp_srcs"]
out = "executable"
name = "main"

[targets.compile_c_cpp_srcs] #example
out = "object"
path = "src" # change it
files = ["*.c", "*.cpp"]

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
      command:
      - gcc ${ FLAGS } -c ${ INPUT } -MMD -MF ${ OUTPUT }.d -o ${ OUTPUT }
      message: Compiling ${ INPUT }

    CXX:
      when:
        ext:
        - cpp
        out: object
      deps: ${ OUTPUT }.d
      format: default_gcc_deps

      flags: CXX_FLAGS
      command:
      - g++ ${ FLAGS } -c ${ INPUT } -MMD -MF ${ OUTPUT }.d -o ${ OUTPUT }
      message: Compiling ${ INPUT }

    LD:
      when:
        out: executable

      flags: LD_FLAGS
      command:
      - g++ ${ INPUT } ${ FLAGS } -o ${ OUTPUT }
      message: Linking ${ OUTPUT }

    AR:
      when:
        out: static-library

      flags: AR_FLAGS
      command:
      - ar ${ FLAGS } ${ OUTPUT } ${ INPUT }
      message: Creating static library ${ OUTPUT }

  flagsets:
    GCC_ALWAYS:
    - "-Wall"
    - "-Wextra"
    - "-Wpedantic"
    - "-Wconversion"

    GCC_DEBUG:
    - "-g"
    - "-O0"
    - "-fsanitize=address,undefined"
    - "-fno-omit-frame-pointer"

    GCC_RELEASE:
    - "-O3"
    - "-DNDEBUG"
    - "-flto"

    C_ALWAYS:
    - "-std=c11"

    CXX_ALWAYS:
    - "-std=c++17"

    AR_ALWAYS:
    - "rcs"

    GCC_WARNINGS_EXTRA:
    - "-Wshadow"
    - "-Wformat=2"
    - "-Wundef"
    - "-Wunreachable-code"

    GCC_SANITIZERS:
    - "-fsanitize=address"
    - "-fsanitize=undefined"
    - "-fno-omit-frame-pointer"

  flags:
    C_FLAGS:
      global:
        always:
        - GCC_ALWAYS
        - C_ALWAYS
        modes:
          debug:
          - GCC_DEBUG
          release:
          - GCC_RELEASE
      platforms:
        windows:
          always: []
          modes: {}
        linux:
          always: []
          modes: {}
        macos:
          always: []
          modes: {}

    CXX_FLAGS:
      global:
        always:
        - GCC_ALWAYS
        - CXX_ALWAYS
        modes:
          debug:
          - GCC_DEBUG
          release:
          - GCC_RELEASE
      platforms:
        windows:
          always: []
          modes: {}
        linux:
          always: []
          modes: {}
        macos:
          always: []
          modes: {}

    LD_FLAGS:
      global:
        always: []
        modes: {}
      platforms:
        windows:
          always: []
          modes: {}
        linux:
          always: []
          modes: {}
        macos:
          always: []
          modes: {}

    AR_FLAGS:
      global:
        always:
        - AR_ALWAYS
        modes: {}
      platforms:
        windows:
          always: []
          modes: {}
        linux:
          always: []
          modes: {}
        macos:
          always: []
          modes: {}
"#;

const DEFAULT_FORMATS: &str =
r#"default_gcc_deps:
  split: " "

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

    if !std::path::Path::new("build-config").exists() {
      fs::create_dir("build-config")?;
    }

    // TODO
    let mut toolchains = File::create("build-config/build_toolchains.yaml")?;
    toolchains.write_all(DEFAULT_TOOLCHAIN.as_bytes())?;

    let mut formats = File::create("build-config/build_formats.yaml")?;
    formats.write_all(DEFAULT_FORMATS.as_bytes())?;
    
    Ok(())
}
