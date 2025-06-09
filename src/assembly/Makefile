C_SRCS = $(shell find $(SRC_DIR) -name '*.c' | sed "s|$(SRC_DIR)/||")
C_HEADERS := $(shell find $(SRC_DIR) -name '*.h' | sed "s|$(SRC_DIR)/||")

C_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.c.o, $(C_SRCS))


ASM_SRCS = $(shell find $(SRC_DIR) -name '*.asm' | sed "s|$(SRC_DIR)/||")

ASM_OBJS = $(patsubst %.asm, $(BUILD_DIR)/%.asm.o, $(ASM_SRCS))

DEPS := $(C_OBJS:.o=.d) $(ASM_OBJS:.o=.d)

TARGET = $(BUILD_DIR)/assembly

.PHONY: all clean

all: $(TARGET)

# Compile C files
$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	@echo "Compiling " $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MF $(@:.o=.d) -c $< -o $@

# Compile asm files
$(BUILD_DIR)/%.asm.o: $(SRC_DIR)/%.asm
	@echo "Assembling " $@
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -MD -MF $(@:.o=.d) -o $@ $<

# link files
$(TARGET): $(C_OBJS) $(ASM_OBJS)
	@echo "Linking " $@
	@$(CC) $(CFLAGS) -o $@ $^

-include $(DEPS)

clean:
	@rm -rf $(BUILD_DIR)