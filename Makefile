include build_scripts/config.mk
include build_scripts/os.mk

SRC_DIR = src
BUILD_DIR = build

SRC_DIR := $(shell pwd)/$(SRC_DIR)
BUILD_DIR := $(shell pwd)/$(BUILD_DIR)

.PHONY: all clean library assembly linker

all: library assembly linker

library:
	@$(MAKE) -C $(SRC_DIR)/lib 				\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		AR=$(AR) ARFLAGS=$(ARFLAGS)			\
		RANLIB=$(RANLIB)					\
		SRC_DIR=$(SRC_DIR)/lib 				\
		LIB=core							\
		BUILD_DIR=$(BUILD_DIR)/lib

assembly: library
	@$(MAKE) -C $(SRC_DIR)/assembly 		\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		SRC_DIR=$(SRC_DIR)/assembly 		\
		BUILD_DIR=$(BUILD_DIR)/assembly		\
		LIB_DIR=$(BUILD_DIR)/lib			\
		LIB=core							\
		EXE_EXT=$(EXE_EXT)

linker: library
	@$(MAKE) -C $(SRC_DIR)/linker 			\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		LIB_DIR=$(BUILD_DIR)/lib			\
		LIB=core							\
		EXE_EXT=$(EXE_EXT)

clean:
	@$(MAKE) -C $(SRC_DIR)/assembly clean 	\
		SRC_DIR=$(SRC_DIR)/assembly 		\
		BUILD_DIR=$(BUILD_DIR)/assembly		\
		EXE_EXT=$(EXE_EXT)
	
	@$(MAKE) -C $(SRC_DIR)/linker clean 	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		EXE_EXT=$(EXE_EXT)
