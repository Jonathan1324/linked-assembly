UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S),Linux)
  EXE_EXT :=
else ifeq ($(UNAME_S),Darwin)
  EXE_EXT :=
else ifneq (,$(findstring MINGW,$(UNAME_S)))
  EXE_EXT := .exe
else
  EXE_EXT :=
endif

ifeq ($(OS),Windows_NT)
    ifeq ($(UNAME_M),AMD64)
        RUST_TARGET = x86_64-pc-windows-gnu
    else ifeq ($(UNAME_M),ARM64)
        RUST_TARGET = aarch64-pc-windows-gnu
    endif
else ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_M),arm64)
        RUST_TARGET = aarch64-apple-darwin
    else
        RUST_TARGET = x86_64-apple-darwin
    endif
else
    # Linux
    ifeq ($(UNAME_M),x86_64)
        RUST_TARGET = x86_64-unknown-linux-gnu
    else ifeq ($(UNAME_M),aarch64)
        RUST_TARGET = aarch64-unknown-linux-gnu
    endif
endif
