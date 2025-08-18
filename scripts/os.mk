UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    UNAME_M := $(shell echo $(PROCESSOR_ARCHITECTURE) | tr '[:upper:]' '[:lower:]')
else ifeq ($(UNAME_S),Darwin)
    UNAME_M := $(shell sysctl -n hw.machine)
else
    # Linux
    UNAME_M := $(shell uname -m)
endif

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
    ifeq ($(UNAME_M),amd64)
        RUST_TARGET = x86_64-pc-windows-gnu
        LDFLAGS += -lws2_32 -luser32 -lkernel32 -lwsock32 -lntdll
    else ifeq ($(UNAME_M),arm64)
        RUST_TARGET = aarch64-pc-windows-gnu
    endif
else ifeq ($(UNAME_S),Darwin)
    ifneq (,$(filter arm64 aarch64,$(UNAME_M)))
        RUST_TARGET = aarch64-apple-darwin
    else
        RUST_TARGET = x86_64-apple-darwin
    endif
else
    # Linux
    ifeq ($(UNAME_M),x86_64)
        RUST_TARGET = x86_64-unknown-linux-gnu
    else ifneq (,$(filter arm64 aarch64,$(UNAME_M)))
        RUST_TARGET = aarch64-unknown-linux-gnu
    endif
endif
