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
    LDFLAGS += -lws2_32 -luser32 -lkernel32 -lwsock32 -lntdll -luserenv
    ifeq ($(UNAME_M),amd64)
        RUST_TARGET = x86_64-pc-windows-gnu
    endif
    ifeq ($(UNAME_M),arm64)
        RUST_TARGET = aarch64-pc-windows-gnu
    endif
endif
ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -lpthread -lm -lc++
    ifneq (,$(filter arm64 aarch64,$(UNAME_M)))
        RUST_TARGET = aarch64-apple-darwin
    else
        RUST_TARGET = x86_64-apple-darwin
    endif
else
    # Linux
    LDFLAGS += -lpthread -ldl -lm
    ifeq ($(UNAME_M),x86_64)
        RUST_TARGET = x86_64-unknown-linux-gnu
    endif
    ifneq (,$(filter arm64 aarch64,$(UNAME_M)))
        RUST_TARGET = aarch64-unknown-linux-gnu
    endif
endif
