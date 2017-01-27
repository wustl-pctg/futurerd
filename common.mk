PROJECT_HOME = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(PROJECT_HOME)/config.mk

OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++

OPT_FLAGS ?= -O3
TOOL_DEBUG ?= 0
LTO ?= 1

INC = -I$(PROJECT_HOME)/include 
FLAGS = -Wall -Wfatal-errors -g $(INC)
CFLAGS += $(FLAGS) -std=c99
CXXFLAGS += $(FLAGS) -std=c++11 -fno-exceptions -fno-rtti
ARFLAGS = rcs

ifeq ($(LTO),1)
  OPT_FLAGS += -flto
  LDFLAGS += -flto
  ARFLAGS += --plugin $(COMPILER_HOME)/lib/LLVMgold.so
endif

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(LIB_DIR)/lib%.a: $(OBJ)
	@mkdir -p $(LIB_DIR)
	ar $(ARFLAGS) $@ $(OBJ)

$(LIB_DIR)/lib%.so: $(OBJ)
	@mkdir -p $(LIB_DIR)
	$(CC) $(OBJ) -shared -o $@
