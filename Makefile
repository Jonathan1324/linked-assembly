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

.PHONY: all clean libcore librust lbf buildtool asmp assembler linker lfs ljoke lhoho

all: libcore librust lbf buildtool asmp assembler linker lfs ljoke lhoho

libcore:
	@"$(MAKE)" -C $(SRC_DIR)/lib 			\
		DEBUG=$(DEBUG)						\
											\
		CC="$(CC)" CFLAGS="$(CFLAGS)" 		\
		CXX="$(CXX)" CXXFLAGS="$(CXXFLAGS)"	\
		AR="$(AR)" ARFLAGS="$(ARFLAGS)"		\
		LDFLAGS="$(LDFLAGS)"				\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		RANLIB="$(RANLIB)"					\
		SRC_DIR="$(SRC_DIR)/lib" 			\
		LIB_DIR="$(LIB_DIR)"				\
		LIB=core							\
		BUILD_DIR="$(BUILD_DIR)/lib"		\
		BIN_DIR="$(BIN_DIR)"				\
		$(ARGS_OS)

librust:
	@"$(MAKE)" -C $(SRC_DIR)/rust			\
		DEBUG=$(DEBUG)						\
											\
		RUSTFLAGS="$(RUSTFLAGS)"			\
		RUSTLIBFLAGS="$(RUSTLIBFLAGS)"		\
		RUST_TARGET="$(RUST_TARGET)"		\
		SRC_DIR=$(SRC_DIR)/rust 			\
		LIB_DIR=$(LIB_DIR)					\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		LIB=rust							\
		BUILD_DIR=$(BUILD_DIR)/rust			\
		BIN_DIR=$(BIN_DIR)					\
		$(ARGS_OS)

buildtool: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/buildtool		\
		DEBUG=$(DEBUG)						\
											\
		RUSTFLAGS="$(RUSTFLAGS)"			\
		RUSTLDFLAGS="$(RUSTLDFLAGS)"		\
		RUST_TARGET="$(RUST_TARGET)"		\
		SRC_DIR=$(SRC_DIR)/buildtool 		\
		BUILD_DIR=$(BUILD_DIR)/buildtool	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lbf: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/lbf  			\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/lbf		 		\
		BUILD_DIR=$(BUILD_DIR)/lbf			\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

ljoke: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/ljoke  		\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/ljoke		 	\
		BUILD_DIR=$(BUILD_DIR)/ljoke		\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lhoho: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/christmas  	\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/christmas		\
		BUILD_DIR=$(BUILD_DIR)/christmas	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

asmp: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/asmp 			\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/asmp		 		\
		BUILD_DIR=$(BUILD_DIR)/asmp			\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

assembler: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/assembler 		\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/assembler 		\
		BUILD_DIR=$(BUILD_DIR)/assembler	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

linker: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/linker 		\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

lfs: libcore librust
	@"$(MAKE)" -C $(SRC_DIR)/filesystem 	\
		DEBUG=$(DEBUG)						\
											\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGSSRC) $(LDFLAGS)"	\
		STRIP="$(STRIP)"					\
		STRIPFLAGS="$(STRIPFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/filesystem 		\
		BUILD_DIR=$(BUILD_DIR)/filesystem	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)					\
		$(ARGS_OS)

clean:
	@"$(MAKE)" -C $(SRC_DIR)/assembler clean 	\
		SRC_DIR=$(SRC_DIR)/assembler 			\
		BUILD_DIR=$(BUILD_DIR)/assembler		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/buildtool clean 	\
		SRC_DIR=$(SRC_DIR)/buildtool 			\
		BUILD_DIR=$(BUILD_DIR)/buildtool		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/asmp clean   		\
		SRC_DIR=$(SRC_DIR)/asmp 				\
		BUILD_DIR=$(BUILD_DIR)/asmp 			\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/lbf clean   		\
		SRC_DIR=$(SRC_DIR)/lbf 					\
		BUILD_DIR=$(BUILD_DIR)/lbf 				\
		EXE_EXT=$(EXE_EXT)
	
	@"$(MAKE)" -C $(SRC_DIR)/linker clean 		\
		SRC_DIR=$(SRC_DIR)/linker 				\
		BUILD_DIR=$(BUILD_DIR)/linker			\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/filesystem clean 	\
		SRC_DIR=$(SRC_DIR)/filesystem 			\
		BUILD_DIR=$(BUILD_DIR)/filesystem		\
		EXE_EXT=$(EXE_EXT)

	@"$(MAKE)" -C $(SRC_DIR)/lib clean 			\
		SRC_DIR=$(SRC_DIR)/lib 					\
		BUILD_DIR=$(BUILD_DIR)/lib

	@"$(MAKE)" -C $(SRC_DIR)/rust clean 		\
		SRC_DIR=$(SRC_DIR)/rust		 			\
		BUILD_DIR=$(BUILD_DIR)/rust				\
		EXE_EXT=$(EXE_EXT)

	@$(RM) $(LIB_DIR)
