PROJECT_HOME = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR = $(PROJECT_HOME)/build
COMPILER_HOME = $(PROJECT_HOME)/llvm-cilk
RUNTIME_HOME = $(PROJECT_HOME)/runtime
RUNTIME_LIB = $(PROJECT_HOME)/build/lib/libcilkrts.a
# RUNTIME_LIB = $(RUNTIME_HOME)/build/libcilkrts.a

TOOL_DEBUG = 1
LTO = 0
