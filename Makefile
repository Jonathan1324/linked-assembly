CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
AS = nasm
ASFLAGS = -f elf64

SRC_DIR = src
BUILD_DIR = build

SRC_DIR := $(shell pwd)/$(SRC_DIR)
BUILD_DIR := $(shell pwd)/$(BUILD_DIR)

.PHONY: all clean assembly

all: assembly

assembly:
	@$(MAKE) -C $(SRC_DIR)/assembly 		\
		CC=$(CC) CFLAGS="$(CFLAGS)" 		\
		AS=$(AS) ASFLAGS="$(ASFLAGS)"		\
		SRC_DIR=$(SRC_DIR)/assembly 		\
		BUILD_DIR=$(BUILD_DIR)/assembly

clean:
	@$(MAKE) -C $(SRC_DIR)/assembly clean 	\
		SRC_DIR=$(SRC_DIR)/assembly 		\
		BUILD_DIR=$(BUILD_DIR)/assembly
