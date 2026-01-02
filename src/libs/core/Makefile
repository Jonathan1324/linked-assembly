C_SRCS = $(shell $(LIST) "$(SRC_DIR)" "*.c")
C_OBJS = $(patsubst %.c, $(BUILD_DIR)/%.c.o, $(C_SRCS))

CPP_SRCS = $(shell $(LIST) "$(SRC_DIR)" "*.cpp")
CPP_OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.cpp.o, $(CPP_SRCS))

DEPS := $(C_OBJS:.o=.d) $(CPP_OBJS:.o=.d) $(ASM_OBJS:.o=.d)

TARGET = $(LIB_DIR)/lib$(LIB).a

CFLAGS	 += -I$(SRC_DIR)
CXXFLAGS += -I$(SRC_DIR)

.PHONY: all clean

all: $(TARGET)

# Compile C files
$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	@echo "Compiling " $<
	@$(MKDIR) "$(dir $@)"
	@$(CC) $(CFLAGS) -MMD -MF $(@:.o=.d) -c $< -o $@

# Compile C++ files
$(BUILD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@echo "Compiling " $<
	@$(MKDIR) "$(dir $@)"
	@$(CXX) $(CXXFLAGS) -MMD -MF $(@:.o=.d) -c $< -o $@

# Create static library
$(TARGET): $(C_OBJS) $(CPP_OBJS)
	@echo "Creating static library " $@
	@$(MKDIR) "$(LIB_DIR)"
	@$(AR) $(ARFLAGS) $@ $^
	@$(RANLIB) $@

-include $(DEPS)

clean:
	@$(RM) "$(BUILD_DIR)"
