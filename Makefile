include build_scripts/config.mk
include build_scripts/os.mk

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SRC_DIR := $(shell pwd)/$(SRC_DIR)
BUILD_DIR := $(shell pwd)/$(BUILD_DIR)

.PHONY: all bin clean library assembly linker

all: bin library assembly linker

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
		BUILD_DIR=$(BUILD_DIR)/lib

assembly: library
	@$(MAKE) -C $(SRC_DIR)/assembly 		\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS=$(LDFLAGS)					\
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
		LDFLAGS=$(LDFLAGS)					\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		LIB_DIR=$(BUILD_DIR)/lib			\
		LIB=core							\
		EXE_EXT=$(EXE_EXT)

bin: assembly linker
	@mkdir -p $(BIN_DIR)
	@cp $(BUILD_DIR)/assembly/assembly$(EXE_EXT) $(BIN_DIR)/assembly$(EXE_EXT)
	@cp $(BUILD_DIR)/linker/linker$(EXE_EXT) $(BIN_DIR)/linker$(EXE_EXT)


clean:
	@$(MAKE) -C $(SRC_DIR)/assembly clean 	\
		SRC_DIR=$(SRC_DIR)/assembly 		\
		BUILD_DIR=$(BUILD_DIR)/assembly		\
		EXE_EXT=$(EXE_EXT)
	
	@$(MAKE) -C $(SRC_DIR)/linker clean 	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		EXE_EXT=$(EXE_EXT)

	@$(MAKE) -C $(SRC_DIR)/lib clean 		\
		SRC_DIR=$(SRC_DIR)/lib 				\
		BUILD_DIR=$(BUILD_DIR)/lib

	@rm -rf $(BUILD_DIR)/bin
