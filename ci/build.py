from dataclasses import dataclass, field
from pathlib import Path
import subprocess
from typing import Optional, Dict
import hashlib
import json
import shutil
import logging
from os import environ
import tomllib

from ci.os import OS, ARCH, getOS, getArch

logger = logging.getLogger("ci")

class BuildCache:
    def __init__(self, cache_file: Path):
        self.cache_file = cache_file
        self.hashes: Dict[str, str] = {}
        self.load()

    def load(self):
        if self.cache_file.exists():
            try:
                self.hashes = json.loads(self.cache_file.read_text())
            except Exception:
                print(f"Warning: Failed to load build cache from {self.cache_file}")
                self.hashes = {}

    def save(self):
        self.cache_file.write_text(json.dumps(self.hashes, indent=4, ensure_ascii=False))

    def get(self, target: Path) -> Optional[str]:
        return self.hashes.get(str(target))

    def update(self, target: Path, hash_value: str):
        self.hashes[str(target)] = hash_value

    def is_up_to_date(self, target: Path, hash_value: str) -> bool:
        if not target.exists():
            return False
        return self.get(target) == hash_value

@dataclass
class Toolchain:
    Compiler_C: Optional[str] = None
    Compiler_C_Dependency_Args: list[str] = None
    Compiler_C_Flags: list[str] = field(default_factory=list)

    Compiler_CPP: Optional[str] = None
    Compiler_CPP_Dependency_Args: list[str] = None
    Compiler_CPP_Flags: list[str] = field(default_factory=list)

    Linker: Optional[str] = None
    Linker_Flags: list[str] = field(default_factory=list)
    Library_Flags: list[str] = field(default_factory=list)

    Libs: list[Path] = field(default_factory=list)
    Lib_Names: list[str] = field(default_factory=list)

    Ar: Optional[str] = None
    Ar_Flags: list[str] = field(default_factory=list)

    Strip: Optional[str] = None
    Strip_Flags: list[str] = field(default_factory=list)

    Rust_Lib_Flags: list[str] = field(default_factory=list)
    Rust_Flags: list[str] = field(default_factory=list)
    Rust_Target: Optional[str] = None

def parse_gcc_dep_file(dep_path: Path) -> list[str]:
    if not dep_path.exists():
        return []
    
    content = dep_path.read_text()
    content = content.replace('\\\n', ' ')
    parts = content.split()
    if not parts: return []
    
    # first part is target
    return parts[1:]

def hash_files(files: list[Path]) -> str:
    hasher = hashlib.new("sha256")

    for file in sorted(files, key=lambda f: str(f)):
        with file.open("rb") as f:
            while chunk := f.read(8192):
                hasher.update(chunk)
                
    return hasher.hexdigest()

def build_c_cpp_sources(toolchain: Toolchain, buildCache: BuildCache, build_dir: Path, source_dir: Path) -> list[Path]:
    patterns = ["*.c", "*.cpp"]

    files: list[Path] = []
    for pattern in patterns:
        files.extend(source_dir.rglob(pattern))

    objects: list[Path] = []
    for file in files:
        rel_path = file.relative_to(source_dir)

        target_path = build_dir / rel_path.with_suffix(".o")
        target_path.parent.mkdir(parents=True, exist_ok=True)
        dep_path = target_path.with_suffix(".d")

        objects.append(target_path)

        compiler: str
        flags: list[str]
        dep_args: list[str]
        if file.suffix == ".c":
            compiler = toolchain.Compiler_C
            flags = toolchain.Compiler_C_Flags
            dep_args = toolchain.Compiler_C_Dependency_Args

        elif file.suffix == ".cpp":
            compiler = toolchain.Compiler_CPP
            flags = toolchain.Compiler_CPP_Flags
            dep_args = toolchain.Compiler_CPP_Dependency_Args

        else:
            raise ValueError(f"Unknown source extension: {file.suffix}")

        try:
            deps = parse_gcc_dep_file(dep_path)
            all_deps = [file, *map(Path, deps)]
            content_hash = hash_files(all_deps)

            if not buildCache.is_up_to_date(target_path, content_hash):
                print(f"Compiling {file} -> {target_path}")
                subprocess.run([compiler, *flags, *dep_args, str(dep_path), "-c", str(file), "-o", str(target_path)], check=True)

                new_deps = parse_gcc_dep_file(dep_path)
                new_all_deps = [file, *map(Path, new_deps)]
                new_content_hash = hash_files(new_all_deps)
                buildCache.update(target_path, new_content_hash)

        except subprocess.CalledProcessError as e:
            logger.error(f"Error: Compilation failed for {file}")
            raise e
        
    return objects

def build_lib(toolchain: Toolchain, buildCache: BuildCache, build_dir: Path, source_dir: Path, out: Path) -> bool:
    info_file = source_dir / "build.info"
    if not info_file.exists():
        logger.warning(f"Warning: build.info missing in {source_dir}")
        return False
    
    info_content = info_file.read_text(encoding="utf-8").strip()

    if info_content == "ignore":
        return False
    
    elif info_content == "rust":
        patterns = ["*.rs"]

        files: list[Path] = []
        for pattern in patterns:
            files.extend(source_dir.rglob(pattern))

        rustc = "rustc" # TODO: hardcoded
        flags = [*toolchain.Rust_Flags, *toolchain.Rust_Lib_Flags]

        file = source_dir / "lib.rs" # TODO: ???

        out.parent.mkdir(parents=True, exist_ok=True)

        try:
            content_hash = hash_files(files)

            if not buildCache.is_up_to_date(out, content_hash):
                print(f"Creating static rust library {out}")
                subprocess.run([rustc, *flags, f"--target={toolchain.Rust_Target}", str(file), "-o", str(out)], check=True)

                buildCache.update(out, content_hash)

        except subprocess.CalledProcessError as e:
            logger.error(f"Error: Creating static library failed for {out}")
            raise e

        return True

    elif info_content == "c,c++":
        objects = build_c_cpp_sources(toolchain, buildCache, build_dir, source_dir)

        ar = toolchain.Ar
        flags = toolchain.Ar_Flags

        out.parent.mkdir(parents=True, exist_ok=True)

        try:
            content_hash = hash_files(objects)

            if not buildCache.is_up_to_date(out, content_hash):
                print(f"Creating static library {out}")
                subprocess.run([ar, *flags, out, *map(str, objects)], check=True)

                buildCache.update(out, content_hash)

        except subprocess.CalledProcessError as e:
            logger.error(f"Error: Creating static library failed for {out}")
            raise e
    
        return True

    else:
        logger.warning(f"Warning: invalid build.info content in {source_dir}")
        return False

def build_tool(debug: bool, os: OS, toolchain: Toolchain, buildCache: BuildCache, build_dir: Path, source_dir: Path, name: str, lib_dir: Path, tpl_dir: Path) -> Optional[str]:
    info_file = source_dir / "build.info"
    if not info_file.exists():
        logger.warning(f"Warning: build.info missing in {source_dir}")
        return None
    
    info_content = info_file.read_text(encoding="utf-8").strip()

    if info_content == "ignore":
        return None
    
    elif info_content == "cargo":
        patterns = ["*.rs"]

        files: list[Path] = []
        for pattern in patterns:
            files.extend(source_dir.rglob(pattern))

        cargo = "cargo" # TODO: hardcoded
        flags = toolchain.Rust_Flags

        env = environ.copy()
        env["LIB_DIR"] = str(lib_dir.absolute())
        env["STATIC_LIBS"] = ",".join(map(str, toolchain.Lib_Names))
        env["RUSTFLAGS"] = " ".join(flags)
        env["CARGO_TARGET_DIR"] = str(build_dir)

        out = build_dir / toolchain.Rust_Target / ("release" if not debug else "debug") / name
        if os == OS.Windows:
            out = out.with_suffix(".exe")

        cmd = [
            cargo,
            "build",
            "--bin", name,
            "--target", toolchain.Rust_Target,
            "--manifest-path", str(source_dir / "Cargo.toml"),
        ]
        if not debug: cmd.append("--release")

        try:
            content_hash = hash_files([*files, *toolchain.Libs])

            if not buildCache.is_up_to_date(out, content_hash):
                print(f"Building cargo executable {out}")
                subprocess.run(cmd, env=env, check=True)

                buildCache.update(out, content_hash)
        
        except subprocess.CalledProcessError as e:
            logger.error(f"Error: Building cargo executable failed for {out}")
            raise e

        if not out.exists():
            raise RuntimeError(f"Cargo build succeeded but binary not found: {out}")
        
        cargo_toml = source_dir / "Cargo.toml"
        pkg_name = tomllib.loads(cargo_toml.read_text())["package"]["name"]

        license_cmd = [
            cargo,
            "license",
            "--do-not-bundle",
            "--json",
            "--manifest-path", str(cargo_toml),
        ]

        result = subprocess.run(
            license_cmd,
            env=env,
            check=True,
            capture_output=True,
            text=True,
        )

        licenses = json.loads(result.stdout)

        filtered = [
            entry for entry in licenses
            if entry["name"] != pkg_name
        ]

        out_file = tpl_dir / f"{name}.txt"
        out_file.parent.mkdir(parents=True, exist_ok=True)

        with out_file.open("w", encoding="utf-8") as f:
            for dep in filtered:
                f.write(f'{dep["name"]} {dep["version"]}: {dep["license"]}\n')

        return out
    
    elif info_content == "c,c++":
        out = build_dir / name
        if os == OS.Windows:
            out = out.with_suffix(".exe")

        objects = build_c_cpp_sources(toolchain, buildCache, build_dir, source_dir)

        linker = toolchain.Linker
        flags = toolchain.Linker_Flags
        lib_flags = toolchain.Library_Flags

        out.parent.mkdir(parents=True, exist_ok=True)

        try:
            content_hash = hash_files([*objects, *toolchain.Libs])

            if not buildCache.is_up_to_date(out, content_hash):
                print(f"Linking {out}")
                subprocess.run([linker, *map(str, objects), *lib_flags, *flags, "-o", str(out)], check=True)

                buildCache.update(out, content_hash)

        except subprocess.CalledProcessError as e:
            logger.error(f"Error: Linking failed for {out}")
            raise e
        
        if not debug:
            strip = toolchain.Strip
            flags = toolchain.Strip_Flags

            try:
                subprocess.run([strip, *flags, str(out)], check=True)
            except subprocess.CalledProcessError as e:
                logger.error(f"Error: Stripping failed for {out}")
                raise e

        return out
    
    else:
        logger.warning(f"Warning: invalid build.info content in {source_dir}")
        return None

def build(debug: bool, os: OS, arch: ARCH, tools: list[str]) -> bool:
    logger.info("Building the project")

    cache: BuildCache = BuildCache(Path(".buildcache.json"))

    toolchain: Toolchain = Toolchain()

    # Tools
    toolchain.Strip = "strip"
    toolchain.Ar = "ar"

    toolchain.Compiler_C_Dependency_Args = ["-MMD", "-MF"]
    toolchain.Compiler_CPP_Dependency_Args = ["-MMD", "-MF"]

    if os == OS.macOS:
        toolchain.Compiler_C = "clang"
        toolchain.Compiler_CPP = "clang++"
        toolchain.Linker = "clang++"
    
    else:
        toolchain.Compiler_C = "gcc"
        toolchain.Compiler_CPP = "g++"
        toolchain.Linker = "g++"

    

    Warning_Flags = ["-Wall", "-Wextra"]
    Optimize_Flags = ["-O2"]
    Debug_Flags = ["-g", "-DDEBUG_BUILD"]
    Release_Flags = ["-DNDEBUG"]
    Security_Flags = ["-fstack-protector-strong", "-D_FORTIFY_SOURCE=2", "-fPIC"]
    Static_Flags = ["-static", "-static-libgcc", "-static-libstdc++"]

    # FLAGS
    toolchain.Compiler_C_Flags.extend(Warning_Flags)

    toolchain.Compiler_CPP_Flags.append("-std=c++17")
    toolchain.Compiler_CPP_Flags.extend(Warning_Flags)

    toolchain.Ar_Flags.append("rcs")

    toolchain.Rust_Lib_Flags.extend(["--crate-type", "staticlib"])

    if os == OS.Linux:
        toolchain.Linker_Flags.append("-Wl,--gc-sections")

        toolchain.Compiler_C_Flags.append("-ffunction-sections")
        toolchain.Compiler_C_Flags.append("-fdata-sections")

        toolchain.Compiler_CPP_Flags.append("-ffunction-sections")
        toolchain.Compiler_CPP_Flags.append("-fdata-sections")

        toolchain.Linker_Flags.append("-lpthread")
        toolchain.Linker_Flags.append("-ldl")
        toolchain.Linker_Flags.append("-lm")

        toolchain.Strip_Flags.append("--strip-unneeded")

        if   arch == ARCH.x86_64: toolchain.Rust_Target = "x86_64-unknown-linux-gnu"
        elif arch == ARCH.ARM64:  toolchain.Rust_Target = "aarch64-unknown-linux-gnu"
    
    elif os == OS.macOS:
        toolchain.Linker_Flags.append("-lpthread")
        toolchain.Linker_Flags.append("-lm")
        toolchain.Linker_Flags.append("-lc++")

        toolchain.Strip_Flags.append("-x")
        toolchain.Strip_Flags.append("-S")

        if   arch == ARCH.x86_64: toolchain.Rust_Target = "x86_64-apple-darwin"
        elif arch == ARCH.ARM64:  toolchain.Rust_Target = "aarch64-apple-darwin"

    elif os == OS.Windows:
        toolchain.Linker_Flags.append("-lws2_32")
        toolchain.Linker_Flags.append("-luser32")
        toolchain.Linker_Flags.append("-lkernel32")
        toolchain.Linker_Flags.append("-lwsock32")
        toolchain.Linker_Flags.append("-lntdll")
        toolchain.Linker_Flags.append("-luserenv")

        if   arch == ARCH.x86_64: toolchain.Rust_Target = "x86_64-pc-windows-gnu"
        elif arch == ARCH.ARM64:  toolchain.Rust_Target = "aarch64-pc-windows-gnullvm" # TODO: Check


    if debug:
        toolchain.Compiler_C_Flags.extend(Debug_Flags)

        toolchain.Compiler_CPP_Flags.extend(Debug_Flags)

        toolchain.Rust_Flags.extend(["-C", "opt-level=0"])

    else:
        toolchain.Compiler_C_Flags.extend(Optimize_Flags)
        toolchain.Compiler_C_Flags.extend(Release_Flags)

        toolchain.Compiler_CPP_Flags.extend(Optimize_Flags)
        toolchain.Compiler_CPP_Flags.extend(Release_Flags)
        toolchain.Compiler_CPP_Flags.extend(Security_Flags)

        toolchain.Rust_Flags.extend(["-C", "opt-level=3"])

        if os != OS.macOS:
            toolchain.Linker_Flags.extend(Static_Flags)


    build_dir = Path("build") / ("debug" if debug else "release")
    
    lib_out_dir = build_dir / "libs"
    tools_build_dir = build_dir / "tools"

    tpl_build_dir = build_dir / "third_party_licenses"
    shutil.rmtree(tpl_build_dir, ignore_errors=True)
    tpl_build_dir.mkdir(parents=True, exist_ok=True)

    source_dir = Path("src")
    libs_srcs = source_dir / "libs"
    tools_srcs = source_dir / "tools"

    binaries_txt = build_dir / "binaries.txt"
    binaries_txt.parent.mkdir(parents=True, exist_ok=True)

    binaries_txt.write_text("")

    toolchain.Library_Flags.append(f"-L{str(lib_out_dir.absolute())}")

    # Libraries
    libs: list[Path] = [lib for lib in libs_srcs.iterdir() if lib.is_dir()]

    ## Include
    Include_Flags: list[str] = []
    for lib in libs:
        path = lib.absolute()
        Include_Flags.append(f"-I{str(path)}")

    toolchain.Compiler_C_Flags.extend(Include_Flags)
    toolchain.Compiler_CPP_Flags.extend(Include_Flags)

    ## Compiling
    for lib in libs:
        name = lib.relative_to(libs_srcs)
        name = str(name)

        lib_build_dir = lib_out_dir / name
        out = lib_out_dir / f"lib{name}.a"
        
        try:
            result = build_lib(toolchain, cache, lib_build_dir, lib, out)
            if result:
                toolchain.Library_Flags.append(f"-l{name}")
                toolchain.Lib_Names.append(name)
                toolchain.Libs.append(out)

        except Exception as e:
            cache.save()
            logger.error(f"Building {lib} failed: {e}")
            return False


    # Tools
    tool_paths: list[Path] = [tool for tool in tools_srcs.iterdir() if tool.is_dir()]
    for tool in tool_paths:
        name = tool.relative_to(tools_srcs)
        name = str(name)
        if tools and name not in tools: continue

        tool_build_dir = tools_build_dir / name

        try:
            out = build_tool(debug, os, toolchain, cache, tool_build_dir, tool, name, lib_out_dir, tpl_build_dir)
            if out is not None:
                with binaries_txt.open("a", encoding="utf-8") as f:
                    f.write(str(out) + "\n")

        except Exception as e:
            cache.save()
            logger.error(f"Building {tool} failed: {e}")
            return False


    cache.save()

    return True

def clean(debug: bool, os: OS, arch: ARCH) -> bool:
    logger.info("Cleaning")

    build_dir = Path("build") / ("debug" if debug else "release")
    
    if build_dir.exists(): shutil.rmtree(str(build_dir), ignore_errors=True)

    return True
