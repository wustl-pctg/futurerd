PROJECT_HOME = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(PROJECT_HOME)/config.mk

OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++

OPT_FLAGS = -O3
DBG_FLAGS = -O0
PROF_FLAGS = -O2
TOOL_DEBUG ?= 0

ifeq ($(mode),release)
	EXTRA_FLAGS += $(OPT_FLAGS)
else ifeq ($(mode),profile)
  EXTRA_FLAGS += $(PROF_FLAGS)
else ifeq ($(mode),debug)
  EXTRA_FLAGS += $(DBG_FLAGS)
else ifeq ($(mode),) # default value
  EXTRA_FLAGS += $(DBG_FLAGS) 
else
  $(error "Invalid mode.")
endif

INC = -I$(PROJECT_HOME)/include
FLAGS = -Wall -Wfatal-errors -g $(INC)
CFLAGS ?= -std=c99
CXXFLAGS ?= -std=c++11 -fno-exceptions -fno-rtti
ARFLAGS = rcs

LTO ?= 1
ifeq ($(LTO),1)
  EXTRA_FLAGS += -flto
  LDFLAGS += -flto
  ARFLAGS += --plugin $(COMPILER_HOME)/lib/LLVMgold.so
endif

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(FLAGS) $(CFLAGS) $(EXTRA_FLAGS) -o $(OBJ_DIR)/$@ -c $<

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(FLAGS) $(CXXFLAGS) $(EXTRA_FLAGS) -o $@ -c $<

%.o: %.cpp
	$(CXX) $(FLAGS) $(CXXFLAGS) $(EXTRA_FLAGS) -o $@ -c $<

$(LIB_DIR)/lib%.a: $(OBJ)
	@mkdir -p $(LIB_DIR)
	ar $(ARFLAGS) $@ $(OBJ)

$(LIB_DIR)/lib%.so: $(OBJ)
	@mkdir -p $(LIB_DIR)
	$(CC) $(OBJ) -shared -o $@
