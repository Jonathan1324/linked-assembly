include build_scripts/config.mk
include build_scripts/os.mk

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SRC_DIR := $(shell pwd)/$(SRC_DIR)
BUILD_DIR := $(shell pwd)/$(BUILD_DIR)
BIN_DIR := $(shell pwd)/$(BIN_DIR)

.PHONY: all clean library asmp assembler linker

all: library asmp assembler linker

library:
	@$(MAKE) -C $(SRC_DIR)/lib 				\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		AR=$(AR) ARFLAGS=$(ARFLAGS)			\
		LDFLAGS=$(LDFLAGS)					\
		RANLIB=$(RANLIB)					\
		SRC_DIR=$(SRC_DIR)/lib 				\
		LIB=core							\
		BUILD_DIR=$(BUILD_DIR)/lib			\
		BIN_DIR=$(BIN_DIR)

asmp: library
	@$(MAKE) -C $(SRC_DIR)/asmp 			\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS=$(LDFLAGS)					\
		SRC_DIR=$(SRC_DIR)/asmp		 		\
		BUILD_DIR=$(BUILD_DIR)/asmp			\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(BUILD_DIR)/lib			\
		LIB=core							\
		EXE_EXT=$(EXE_EXT)

assembler: library
	@$(MAKE) -C $(SRC_DIR)/assembler 		\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS=$(LDFLAGS)					\
		SRC_DIR=$(SRC_DIR)/assembler 		\
		BUILD_DIR=$(BUILD_DIR)/assembler	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(BUILD_DIR)/lib			\
		LIB=core							\
		EXE_EXT=$(EXE_EXT)

linker: library
	@$(MAKE) -C $(SRC_DIR)/linker 			\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS=$(LDFLAGS)					\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(BUILD_DIR)/lib			\
		LIB=core							\
		EXE_EXT=$(EXE_EXT)


clean:
	@$(MAKE) -C $(SRC_DIR)/assembler clean 	\
		SRC_DIR=$(SRC_DIR)/assembler 		\
		BUILD_DIR=$(BUILD_DIR)/assembler		\
		EXE_EXT=$(EXE_EXT)
	
	@$(MAKE) -C $(SRC_DIR)/linker clean 	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		EXE_EXT=$(EXE_EXT)

	@$(MAKE) -C $(SRC_DIR)/lib clean 		\
		SRC_DIR=$(SRC_DIR)/lib 				\
		BUILD_DIR=$(BUILD_DIR)/lib

	@rm -rf $(BUILD_DIR)/bin
