BENCH_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(BENCH_DIR)/../common.mk

# APP_TOOL_ONLY_FLAGS = -fcilktool
# APP_RD_FLAGS = $(APP_TOOL_ONLY_FLAGS) -fsanitize=thread

# Why doesn't clang pick this up automatically?
INC += -I$(PROJECT_HOME)/llvm-cilk/include -I$(PROJECT_HOME)/src/
INC += -I$(PROJECT_HOME)/cilkrts/include/
LIB = $(LIB_DIR)/libfuturerd.a

APPFLAGS = -fcilkplus -fcilk-no-inline
BASEFLAGS =
BAGFLAGS = -fcilktool
RDFLAGS = -fsanitize=thread -DRACE_DETECT

CFLAGS += $(APPFLAGS) $(INC)
CXXFLAGS += $(APPFLAGS) $(INC)
LDFLAGS += $(LIB) $(RUNTIME_LIB) -ldl -lpthread
