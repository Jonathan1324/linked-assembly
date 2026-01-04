ifndef OS_NAME
$(error OS_NAME is not set. Must be one of: windows, macos, linux)
endif

ifndef ARCH
$(error ARCH is not set. Must be one of: x86_64, arm64)
endif

include scripts/config.mk
include scripts/os.mk

SRC_DIR = src
BIN_DIR = bin

ifeq ($(DEBUG),1)
	BUILD_DIR = build/debug
else
	BUILD_DIR = build/release
endif

SRC_DIR := $(shell $(PWD))/$(SRC_DIR)
BUILD_DIR := $(shell $(PWD))/$(BUILD_DIR)
BIN_DIR := $(BUILD_DIR)

LIB_DIR=$(BUILD_DIR)/libs

LDFLAGSSRC = -L$(LIB_DIR) -lcore -lrust
RUSTLDFLAGS = -lstatic=core -lstatic=rust

.PHONY: all clean libcore librust lasm lasmp lnk lbt lfs lbf ljoke lhoho

all:

all_tools: libcore librust lasm lasmp lnk lbt lfs lbf ljoke lhoho

libcore:
	@"$(MAKE)" -C $(SRC_DIR)/libs/core 		\
		DEBUG=$(DEBUG)						\
											\
		CC="$(CC)" CFLAGS="$(CFLAGS)" 		\
		CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"	\
		AR="$(AR)" ARFLAGS="$(ARFLAGS)"		\
		LDFLAGS="$(LDFLAGS)"				\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		RANLIB="$(RANLIB)"					\
		SRC_DIR="$(SRC_DIR)/libs/core" 		\
		LIB_DIR="$(LIB_DIR)"				\
		LIB=core							\
		BUILD_DIR="$(BUILD_DIR)/libs/core"	\
		BIN_DIR="$(BIN_DIR)"				\
		$(ARGS_OS)

librust:
	@"$(MAKE)" -C $(SRC_DIR)/libs/rust		\
		DEBUG=$(DEBUG)						\
											\
		RUSTFLAGS="$(RUSTFLAGS)"			\
		RUSTLIBFLAGS="$(RUSTLIBFLAGS)"		\
		RUST_TARGET="$(RUST_TARGET)"		\
		SRC_DIR=$(SRC_DIR)/libs/rust 		\
		LIB_DIR=$(LIB_DIR)					\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		LIB=rust							\
		BUILD_DIR=$(BUILD_DIR)/libs/rust	\
		BIN_DIR=$(BIN_DIR)					\
		$(ARGS_OS)

lasm: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/lasm 	\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/tools/lasm 		\
		BUILD_DIR=$(BUILD_DIR)/tools/lasm	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lasmp: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/lasmp 	\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/tools/lasmp		\
		BUILD_DIR=$(BUILD_DIR)/tools/lasmp	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lnk: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/lnk 		\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/tools/lnk 		\
		BUILD_DIR=$(BUILD_DIR)/tools/lnk	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lbt: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/lbt		\
		DEBUG=$(DEBUG)						\
											\
		RUSTFLAGS="$(RUSTFLAGS)"			\
		RUSTLDFLAGS="$(RUSTLDFLAGS)"		\
		RUST_TARGET="$(RUST_TARGET)"		\
		SRC_DIR=$(SRC_DIR)/tools/lbt 		\
		BUILD_DIR=$(BUILD_DIR)/tools/lbt	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lfs: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/lfs 		\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/tools/lfs 		\
		BUILD_DIR=$(BUILD_DIR)/tools/lfs	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lbf: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/lbf  	\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/tools/lbf		\
		BUILD_DIR=$(BUILD_DIR)/tools/lbf	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

ljoke: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/ljoke  	\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/tools/ljoke		\
		BUILD_DIR=$(BUILD_DIR)/tools/ljoke	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lhoho: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/tools/lhoho  	\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/tools/lhoho		\
		BUILD_DIR=$(BUILD_DIR)/tools/lhoho	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

clean:
	@"$(MAKE)" -C $(SRC_DIR)/libs/core clean 	\
		SRC_DIR=$(SRC_DIR)/libs/core 			\
		BUILD_DIR=$(BUILD_DIR)/libs/core

	@"$(MAKE)" -C $(SRC_DIR)/libs/rust clean 	\
		SRC_DIR=$(SRC_DIR)/libs/rust		 	\
		BUILD_DIR=$(BUILD_DIR)/libs/rust
	
	@"$(MAKE)" -C $(SRC_DIR)/tools/lasmp clean  \
		SRC_DIR=$(SRC_DIR)/tools/lasmp 			\
		BUILD_DIR=$(BUILD_DIR)/tools/lasmp 		\
		EXE_EXT=$(EXE_EXT)
	
	@"$(MAKE)" -C $(SRC_DIR)/tools/lasm clean 	\
		SRC_DIR=$(SRC_DIR)/tools/lasm 			\
		BUILD_DIR=$(BUILD_DIR)/tools/lasm		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/tools/lnk clean 	\
		SRC_DIR=$(SRC_DIR)/tools/lnk 			\
		BUILD_DIR=$(BUILD_DIR)/tools/lnk		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/tools/lbt clean 	\
		SRC_DIR=$(SRC_DIR)/tools/lbt 			\
		BUILD_DIR=$(BUILD_DIR)/tools/lbt		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/tools/lfs clean 	\
		SRC_DIR=$(SRC_DIR)/tools/lfs 			\
		BUILD_DIR=$(BUILD_DIR)/tools/lfs		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/tools/lbf clean   	\
		SRC_DIR=$(SRC_DIR)/tools/lbf 			\
		BUILD_DIR=$(BUILD_DIR)/tools/lbf 		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/tools/ljoke clean 	\
		SRC_DIR=$(SRC_DIR)/tools/ljoke 			\
		BUILD_DIR=$(BUILD_DIR)/tools/ljoke		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/tools/lhoho clean  \
		SRC_DIR=$(SRC_DIR)/tools/lhoho 			\
		BUILD_DIR=$(BUILD_DIR)/tools/lhoho 		\
		EXE_EXT=$(EXE_EXT)

	@$(RM) $(LIB_DIR)
