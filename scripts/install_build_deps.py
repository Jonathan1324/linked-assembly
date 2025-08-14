#!/usr/bin/env python3
import os
import platform
import subprocess
import sys

def run(cmd):
    print(f"> {cmd}")
    subprocess.check_call(cmd, shell=True)

def install_linux():
    run("sudo apt-get update")
    run("sudo apt-get install -y build-essential nasm bazelisk")
    install_rust()

def install_macos():
    run("brew install make nasm bazelisk")
    install_rust()

def install_windows():
    run("choco install make nasm bazelisk 7zip -y")
    install_rust()

def install_rust():
    if not shutil.which("rustc"):
        if platform.system() == "Windows":
            run("powershell -Command \"iwr https://win.rustup.rs -UseBasicParsing | iex\"")
        else:
            run("curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y")
    run("cargo install cbindgen")

if __name__ == "__main__":
    import shutil
    system = platform.system().lower()
    if system == "linux":
        install_linux()
    elif system == "darwin":
        install_macos()
    elif system == "windows":
        install_windows()
    else:
        sys.exit(f"Unsupported OS: {system}")
