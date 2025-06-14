UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ARCH ?= $(UNAME_M)

AS = nasm
ASFLAGS = 
CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

ifeq ($(UNAME_S),Darwin)
	CC = clang
	CXX = clang++
	
	ASFLAGS += -f macho64
	CFLAGS += -arch $(ARCH)
	CXXFLAGS += -arch $(ARCH)
else
	ASFLAGS += -f elf64

#	ARM
	ifneq (,$(filter arm64 aarch64,$(ARCH)))
		CFLAGS += -march=armv8-a
		CXXFLAGS += -march=armv8-a
	else ifneq (,$(filter armv7l armhf,$(ARCH)))
		CFLAGS += -march=armv7-a -mfloat-abi=hard -mfpu=neon
		CXXFLAGS += -march=armv7-a -mfloat-abi=hard -mfpu=neon
	else ifneq (,$(filter armv6l,$(ARCH)))
		CFLAGS += -march=armv6 -mfloat-abi=hard -mfpu=vfp
		CXXFLAGS += -march=armv6 -mfloat-abi=hard -mfpu=vfp

#	x86
 	else ifneq (,$(filter x86_64,$(ARCH)))
		CFLAGS += -m64
		CXXFLAGS += -m64
	else ifneq (,$(filter i686 i386,$(ARCH)))
		CFLAGS += -m32
		CXXFLAGS += -m32

#	Unknown
	else
		$(warning Not known architecture: $(ARCH))
	endif
endif