include scripts/config.mk
include scripts/os.mk

SRC_DIR = src
BIN_DIR = bin

ifeq ($(DEBUG),1)
	BUILD_DIR = build/debug
else
	BUILD_DIR = build/release
endif

SRC_DIR := $(shell pwd)/$(SRC_DIR)
BUILD_DIR := $(shell pwd)/$(BUILD_DIR)
BIN_DIR := $(BUILD_DIR)

LIB_DIR=$(BUILD_DIR)/libs

LDFLAGSSRC = -L$(LIB_DIR) -lcore -lrust

.PHONY: all clean libcore librust asmp assembler linker

all: libcore librust asmp assembler linker

libcore:
	@$(MAKE) -C $(SRC_DIR)/lib 				\
		DEBUG=$(DEBUG)						\
											\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		AR=$(AR) ARFLAGS=$(ARFLAGS)			\
		LDFLAGS=$(LDFLAGS)					\
		RANLIB=$(RANLIB)					\
		SRC_DIR=$(SRC_DIR)/lib 				\
		LIB_DIR=$(LIB_DIR)					\
		LIB=core							\
		BUILD_DIR=$(BUILD_DIR)/lib			\
		BIN_DIR=$(BIN_DIR)

librust:
	@$(MAKE) -C $(SRC_DIR)/rust				\
		DEBUG=$(DEBUG)						\
											\
		RUSTFLAGS="$(RUSTFLAGS)"			\
		SRC_DIR=$(SRC_DIR)/rust 			\
		LIB_DIR=$(LIB_DIR)					\
		LIB=rust							\
		BUILD_DIR=$(BUILD_DIR)/rust			\
		BIN_DIR=$(BIN_DIR)

asmp: libcore librust
	@$(MAKE) -C $(SRC_DIR)/asmp 			\
		DEBUG=$(DEBUG)						\
											\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGS) $(LDFLAGSSRC)"	\
		SRC_DIR=$(SRC_DIR)/asmp		 		\
		BUILD_DIR=$(BUILD_DIR)/asmp			\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)

assembler: libcore librust
	@$(MAKE) -C $(SRC_DIR)/assembler 		\
		DEBUG=$(DEBUG)						\
											\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGS) $(LDFLAGSSRC)"	\
		SRC_DIR=$(SRC_DIR)/assembler 		\
		BUILD_DIR=$(BUILD_DIR)/assembler	\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)

linker: libcore librust
	@$(MAKE) -C $(SRC_DIR)/linker 			\
		DEBUG=$(DEBUG)						\
											\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		CXX=$(CXX) CXXFLAGS="$(CXXFLAGS)"	\
		LDFLAGS="$(LDFLAGS) $(LDFLAGSSRC)"	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		BIN_DIR=$(BIN_DIR)					\
		LIB_DIR=$(LIB_DIR)					\
		EXE_EXT=$(EXE_EXT)


clean:
	@$(MAKE) -C $(SRC_DIR)/assembler clean 	\
		SRC_DIR=$(SRC_DIR)/assembler 		\
		BUILD_DIR=$(BUILD_DIR)/assembler	\
		EXE_EXT=$(EXE_EXT)

	@$(MAKE) -C $(SRC_DIR)/asmp clean   	\
		SRC_DIR=$(SRC_DIR)/asmp 			\
		BUILD_DIR=$(BUILD_DIR)/asmp 		\
		EXE_EXT=$(EXE_EXT)
	
	@$(MAKE) -C $(SRC_DIR)/linker clean 	\
		SRC_DIR=$(SRC_DIR)/linker 			\
		BUILD_DIR=$(BUILD_DIR)/linker		\
		EXE_EXT=$(EXE_EXT)

	@$(MAKE) -C $(SRC_DIR)/lib clean 		\
		SRC_DIR=$(SRC_DIR)/lib 				\
		BUILD_DIR=$(BUILD_DIR)/lib

	@$(MAKE) -C $(SRC_DIR)/rust clean 		\
		SRC_DIR=$(SRC_DIR)/rust 			\
		BUILD_DIR=$(BUILD_DIR)/rust

	@rm -rf $(LIB_DIR)
