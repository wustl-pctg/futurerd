TEST_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(TEST_DIR)/../common.mk

# APP_TOOL_ONLY_FLAGS = -fcilktool
# APP_RD_FLAGS = $(APP_TOOL_ONLY_FLAGS) -fsanitize=thread

# Why doesn't clang pick this up automatically?
INC += -I$(PROJECT_HOME)/llvm-cilk/include -I$(PROJECT_HOME)/src/
INC += -I$(PROJECT_HOME)/cilkrts/include/

APPFLAGS = -fcilkplus -fcilktool -fcilk-no-inline
APPFLAGS += -fsanitize=thread -fsanitize-blacklist=../../blacklist.txt

CFLAGS += $(APPFLAGS)
CXXFLAGS += $(APPFLAGS)
LDFLAGS += -ldl -lpthread $(RUNTIME_LIB)
