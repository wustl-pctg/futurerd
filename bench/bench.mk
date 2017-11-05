BENCH_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(BENCH_DIR)/../common.mk

# Why doesn't clang pick this up automatically?
# INC += -I$(PROJECT_HOME)/llvm-cilk/include
# INC += -I$(PROJECT_HOME)/cilkrts/include
INC += -I$(PROJECT_HOME)/src    # for future.hpp
INC += -I$(BUILD_DIR)/include   # for runtime include
LIB = $(LIB_DIR)/librd.a
LIB += $(RUNTIME_LIB) 

# FUTURE_TYPE is already defined in common.mk, used both by the tool and the
# user code.  In the user code, when race detection is on, if the tool is compiled
# to use STRUCTURED_FUTURES, we have to use STRUCTURED_FUTURE bench.  If the
# tool is compiled to use NONBLOCKING_FUTURES, either construction of bench works. 
# Thus, adding -DSTRUCTURED_FUTURES is the only valid additional flag.
APPFLAGS = -fcilkplus -fcilk-no-inline
# APPFLAGS += -DSTRUCTURED_FUTURES
BAGFLAGS = -fcilktool
# Uncomment to enable race detection
# RDFLAGS = -fsanitize=thread -DRACE_DETECT 

CFLAGS += $(APPFLAGS) $(RDFLAGS)
CXXFLAGS += $(APPFLAGS) $(RDFLAGS)
LDFLAGS += $(LIB) -lm -ldl -lpthread -lrt
