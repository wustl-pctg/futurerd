PROJECT_HOME = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
include $(PROJECT_HOME)/config.mk

OBJ_DIR = $(BUILD_DIR)/obj
LIB_DIR = $(BUILD_DIR)/lib

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++

OPT_FLAGS = -O3
DBG_FLAGS = -O0
PROF_FLAGS = -O2
INC = -I$(RUNTIME_HOME)/include
FLAGS = -Wall -Wfatal-errors -g $(INC)
#FLAGS += -DNONBLOCKING_FUTURES
FLAGS += -DSTRUCTURED_FUTURES
ARFLAGS = rcs

TOOL_DEBUG ?= 0

ifeq ($(mode),release)
	FLAGS += $(OPT_FLAGS)
else ifeq ($(mode),profile)
  FLAGS += $(PROF_FLAGS)
else ifeq ($(mode),debug)
  FLAGS += $(DBG_FLAGS)
else ifeq ($(mode),) # default value
  FLAGS += $(DBG_FLAGS) 
else
  $(error "Invalid mode.")
endif

LTO ?= 1
ifeq ($(LTO),1)
  FLAGS += -flto
  LDFLAGS += -flto
  ARFLAGS += --plugin $(COMPILER_HOME)/lib/LLVMgold.so
endif

CFLAGS ?= $(FLAGS) -std=c99
CXXFLAGS ?= $(FLAGS) -std=c++11 -fno-exceptions -fno-rtti

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $<

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(LIB_DIR)/lib%.a: $(OBJ)
	@mkdir -p $(LIB_DIR)
	ar $(ARFLAGS) $@ $(OBJ)

$(LIB_DIR)/lib%.so: $(OBJ)
	@mkdir -p $(LIB_DIR)
	$(CC) $(OBJ) -shared -o $@
