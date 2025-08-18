TARGET = $(BUILD_DIR)/buildtool$(EXE_EXT)
RUST_SRCS = $(shell find $(SRC_DIR) -name '*.rs' | sed "s|$(SRC_DIR)/||")

.PHONY: all clean

all: $(TARGET) record

$(TARGET): $(RUST_SRCS)
	@echo "Compiling Rust source " $@
	@mkdir -p $(BUILD_DIR)
	@rustc $(RUSTFLAGS) --target=$(RUST_TARGET) $(SRC_DIR)/main.rs $(RUSTLDFLAGS) -o $@
#	Rust just uses one file and modules inside of it

record:
	@echo $(TARGET) >> $(BIN_DIR)/binaries.txt

clean:
	@rm -rf $(BUILD_DIR)
