TARGET = $(LIB_DIR)/lib$(LIB).a
RUST_SRCS = $(shell find $(SRC_DIR) -name '*.rs' | sed "s|$(SRC_DIR)/||")

ifeq ($(OS),Windows_NT)
  ifeq ($(ARCH),arm64)
    RUST_TARGET = aarch64-pc-windows-gnu
  else
    RUST_TARGET = x86_64-pc-windows-gnu
  endif
else ifeq ($(UNAME_S),Darwin)
  ifeq ($(ARCH),arm64)
    RUST_TARGET = aarch64-apple-darwin
  else
    RUST_TARGET = x86_64-apple-darwin
  endif
else
  ifeq ($(ARCH),arm64)
    RUST_TARGET = aarch64-unknown-linux-gnu
  else
    RUST_TARGET = x86_64-unknown-linux-gnu
  endif
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(RUST_SRCS)
	@echo "Compiling Rust static library"
	@mkdir -p $(LIB_DIR)
	@rustc $(RUSTFLAGS) --target $(RUST_TARGET) $(SRC_DIR)/lib.rs -o $@
#	Rust just uses one file and modules inside of it
#	@cbindgen --config cbindgen.toml --crate rustlib --output $(LIB_DIR)/rustlib.h

clean:
	@rm -rf $(BUILD_DIR)
	
