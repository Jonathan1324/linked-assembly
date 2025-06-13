include build_scripts/config.mk

SRC_DIR = src
BUILD_DIR = build

SRC_DIR := $(shell pwd)/$(SRC_DIR)
BUILD_DIR := $(shell pwd)/$(BUILD_DIR)

.PHONY: all clean assembly linker

all: assembly linker

assembly:
	@$(MAKE) -C $(SRC_DIR)/assembly 		\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		SRC_DIR=$(SRC_DIR)/assembly 		\
		BUILD_DIR=$(BUILD_DIR)/assembly

linker:
	@$(MAKE) -C $(SRC_DIR)/linker 			\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker

clean:
	@$(MAKE) -C $(SRC_DIR)/linker clean 	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker
