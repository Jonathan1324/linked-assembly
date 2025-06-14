UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
  EXE_EXT :=
else ifeq ($(UNAME_S),Darwin)
  EXE_EXT :=
else ifneq (,$(findstring MINGW,$(UNAME_S)))
  EXE_EXT := .exe
else
  EXE_EXT :=
endif