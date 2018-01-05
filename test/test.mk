TEST_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(TEST_DIR)/../common.mk

# APP_TOOL_ONLY_FLAGS = -fcilktool
# APP_RD_FLAGS = $(APP_TOOL_ONLY_FLAGS) -fsanitize=thread

# Why doesn't clang pick this up automatically?
INC += -I$(PROJECT_HOME)/llvm-cilk/include -I$(PROJECT_HOME)/src/
INC += -I$(PROJECT_HOME)/cilkrts/include/

APPFLAGS = -fcilkplus -fcilktool -fcilk-no-inline -DRACE_DETECT
APPFLAGS += -fsanitize=thread #-fsanitize-blacklist=../../blacklist.txt

# This really only matters for optimized builds, which won't use a
# frame pointer register. This makes __builtin_frame_address return 0,
# which we use in __tsan_func_exit to clear the shadow stack memory.
APPFLAGS += -fno-omit-frame-pointer


CFLAGS += $(APPFLAGS)
CXXFLAGS += $(APPFLAGS)
LDFLAGS += -ldl -lpthread $(RUNTIME_LIB)
