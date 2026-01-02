ifeq ($(OS_NAME), windows)
  EXE_EXT := .exe
else
  EXE_EXT :=
endif

ifeq ($(OS_NAME),windows)
    LDFLAGS += -lws2_32 -luser32 -lkernel32 -lwsock32 -lntdll -luserenv
    ifeq ($(ARCH),x86_64)
        RUST_TARGET = x86_64-pc-windows-gnu
    endif
    ifeq ($(ARCH),arm64)
        RUST_TARGET = aarch64-pc-windows-gnullvm
    endif
endif
ifeq ($(OS_NAME),macos)
    LDFLAGS += -lpthread -lm -lc++
    ifeq ($(ARCH),x86_64)
        RUST_TARGET = x86_64-apple-darwin
    endif
    ifeq ($(ARCH),arm64)
        RUST_TARGET = aarch64-apple-darwin
    endif
endif
ifeq ($(OS_NAME),linux)
    LDFLAGS += -lpthread -ldl -lm
    ifeq ($(ARCH),x86_64)
        RUST_TARGET = x86_64-unknown-linux-gnu
    endif
    ifeq ($(ARCH),arm64)
        RUST_TARGET = aarch64-unknown-linux-gnu
    endif
endif

ifeq ($(OS_NAME),windows)
    SHELL := cmd.exe

    PWD = cd
    LIST = $(shell $(PWD))/scripts/list.bat
    MKDIR = $(shell $(PWD))/scripts/mkdir.bat
    RM = $(shell $(PWD))/scripts/rm.bat
    APPEND = $(shell $(PWD))/scripts/append.bat

    ARGS_OS = PWD="$(PWD)" LIST="$(LIST)" MKDIR="$(MKDIR)" RM="$(RM)" APPEND="$(APPEND)"
else
    PWD = pwd
    LIST = $(shell $(PWD))/scripts/list.sh
    MKDIR = mkdir -p
    RM = rm -rf
    APPEND = $(shell $(PWD))/scripts/append.sh

    ARGS_OS = PWD="$(PWD)" LIST="$(LIST)" MKDIR="$(MKDIR)" RM="$(RM)" APPEND="$(APPEND)"
endif
