ARCH = x86_64

AS = nasm
ASFLAGS = 
CC = gcc
CFLAGS = -arch $(ARCH) -Wall -Wextra -O2 -g

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	ASFLAGS += -f macho64
else
	ASFLAGS += -f elf64
endif