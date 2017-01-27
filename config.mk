PROJECT_HOME = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR = $(PROJECT_HOME)/build
COMPILER_HOME = $(PROJECT_HOME)/llvm-cilk
RUNTIME_LIB = $(COMPILER_HOME)/lib/libcilkrts.a

OPT_FLAGS = -O0
TOOL_DEBUG = 1
LTO = 1
