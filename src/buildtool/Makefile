TARGET = $(BUILD_DIR)/$(RUST_TARGET)/release/lbt$(EXE_EXT)
RUST_SRCS = $(shell $(LIST) $(SRC_DIR) "*.rs")

ifeq ($(WINDOWS_NATIVE),1)
  	WIN_BUILD_DIR := $(subst /,\,$(BUILD_DIR))
endif

.PHONY: all clean

all: $(TARGET) record

$(TARGET): $(RUST_SRCS)
	@echo "Compiling Rust source " $@
	@$(MKDIR) $(BUILD_DIR)

ifeq ($(WINDOWS_NATIVE),1)
	@powershell -Command "$$env:RUSTFLAGS='$(RUSTFLAGS)'; $$env:CARGO_TARGET_DIR='$(WIN_BUILD_DIR)'; cargo build --release --target $(RUST_TARGET) --manifest-path '$(SRC_DIR)/Cargo.toml'"
else
	@RUSTFLAGS="$(RUSTFLAGS)" \
	  CARGO_TARGET_DIR="$(BUILD_DIR)" \
	  cargo build --release --target=$(RUST_TARGET) --manifest-path=$(SRC_DIR)/Cargo.toml
endif

#	@rustc $(RUSTFLAGS) --target=$(RUST_TARGET) $(SRC_DIR)/main.rs $(RUSTLDFLAGS) -o $@
#	Rust just uses one file and modules inside of it

record:
	@$(MKDIR) $(BUILD_DIR)
	
ifeq ($(WINDOWS_NATIVE),1)
	@cmd /C "cargo license --do-not-bundle | findstr /V \"linked-assembly-buildtool\"" > "$(BUILD_DIR)/LICENSES.txt"
else
	@cargo license --do-not-bundle | grep -v "linked-assembly-buildtool" > "$(BUILD_DIR)/LICENSES.txt"
endif

	@$(APPEND) "$(BUILD_DIR)/LICENSES.txt" "$(BIN_DIR)/licenses.txt"
	@$(APPEND) "$(TARGET)" "$(BIN_DIR)/binaries.txt"

clean:
	@$(RM) "$(BUILD_DIR)"
