TARGET = $(LIB_DIR)/lib$(LIB).a
RUST_SRCS = $(shell find $(SRC_DIR) -name '*.rs' | sed "s|$(SRC_DIR)/||")

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(RUST_SRCS)
	@echo "Compiling Rust static library " $@
	@mkdir -p $(LIB_DIR)
	@rustc $(RUSTFLAGS) --target=$(RUST_TARGET) $(SRC_DIR)/lib.rs -o $@
#	Rust just uses one file and modules inside of it
#	@cbindgen --config cbindgen.toml --crate rustlib --output $(LIB_DIR)/rustlib.h

clean:
	@rm -rf $(BUILD_DIR)
	