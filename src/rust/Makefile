TARGET = $(LIB_DIR)/lib$(LIB).a
RUST_SRCS = $(shell $(LIST) "$(SRC_DIR)" "*.rs")

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(RUST_SRCS)
	@echo "Compiling Rust static library " $@
	@$(MKDIR) $(LIB_DIR)
	@rustc $(RUSTFLAGS) $(RUSTLIBFLAGS) --target=$(RUST_TARGET) $(SRC_DIR)/lib.rs -o $@
#	Rust just uses one file and modules inside of it
#	@cbindgen --config cbindgen.toml --crate rustlib --output $(LIB_DIR)/rustlib.h

clean:
	@$(RM) "$(BUILD_DIR)"
