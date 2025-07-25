# ---------------------------------------
# CONFIGURATION BEGIN
# ---------------------------------------

# Assembler Config
ASSEMBLER_MAIN := src/assembler/main_assembler.c
ASSEMBLER_SOURCE := src/assembler/assembler.c
ASSEMBLER_TARGET := assembler

# Project Config
PROJECT_SOURCE_C := src/main.c src/helper_messages.c
PROJECT_SOURCE_CPP := src/tinyRiscMain.cpp
PROJECT_HEADERS_C := include/helper_messages.h
PROJECT_HEADERS_CPP := include/alu.hpp include/cu.hpp include/multiplexer.hpp include/pc.hpp include/register_file.hpp include/tinyRISC.hpp /include/risc/terminal.hpp
PROJECT_TARGET := project

# Path to yur systemc installation
SCPATH = $(SYSTEMC_HOME)

# Additional flags for the compiler
LDFLAGS := 
ASSEMBLER_FLAGS_C :=
PROJECT_FLAGS_CXX := -std=c++14 -Iinclude -I$(SCPATH)/include -L$(SCPATH)/lib -lsystemc -lm -Wl,-rpath=$(SCPATH)/lib

# ---------------------------------------
# CONFIGURATION END
# ---------------------------------------

# Determine if clang or gcc is available
CC := $(shell command -v gcc || command -v clang)
CXX := $(shell command -v g++ || command -v clang++)

ifeq ($(strip $(CC)),)
    $(error Neither clang nor gcc is available. Exiting.)
endif
ifeq ($(strip $(CXX)),)
    $(error Neither clang++ nor g++ is available. Exiting.)
endif

# Add rpath except for MacOS
UNAME_S := $(shell uname -s)

ifneq ($(UNAME_S), Darwin)
    PROJECT_FLAGS_CXX += -Wl,-rpath=$(SCPATH)/lib
endif


# Default to release build for both app and library

all: $(ASSEMBLER_TARGET) $(PROJECT_TARGET)

# Assembler
debug-assembler: ASSEMBLER_FLAGS_C += -g
debug-assembler: $(ASSEMBLER_TARGET)

release-assembler: ASSEMBLER_FLAGS_C += -O2
release-assembler: $(ASSEMBLER_TARGET)

$(ASSEMBLER_TARGET): $(ASSEMBLER_MAIN) $(ASSEMBLER_SOURCE)
	$(CC) $(ASSEMBLER_FLAGS_C) -o $@ $(ASSEMBLER_MAIN) $(ASSEMBLER_SOURCE)

# Project
debug-project: PROJECT_FLAGS_CXX += -g
debug-project: $(PROJECT_TARGET)

release-project: PROJECT_FLAGS_CXX += -O2
release-project: $(PROJECT_TARGET)

$(PROJECT_TARGET): $(PROJECT_SOURCE_C) $(PROJECT_HEADERS_C)
	$(CXX) $(LDFLAGS) -o $@ $(PROJECT_SOURCE_C) $(PROJECT_SOURCE_CPP) $(PROJECT_FLAGS_CXX)


# clean up
clean:
	rm -f $(ASSEMBLER_TARGET) $(PROJECT_TARGET)

.PHONY: all assembler debug-assembler release-assembler project debug-project release-project clean
