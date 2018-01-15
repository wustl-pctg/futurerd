BENCH_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(BENCH_DIR)/../common.mk

INC += -I$(PROJECT_HOME)/src    # for future.hpp
INC += -I$(BUILD_DIR)/include   # for runtime include
INC += -I$(BENCH_DIR)           # for util stuff
RDLIB = $(LIB_DIR)/librd-$(ftype).a
LIB = $(RDLIB) $(RUNTIME_LIB)

# FUTURE_TYPE is already defined in common.mk, used both by the tool and the
# user code.  In the user code, when race detection is on, if the tool is compiled
# to use STRUCTURED_FUTURES, we have to use STRUCTURED_FUTURE bench.  If the
# tool is compiled to use NONBLOCKING_FUTURES, either construction of bench works. 
# Thus, adding -DSTRUCTURED_FUTURES is the only valid additional flag.
# XXX: This is no longer necessary. Just use "make ftype=nonblock" (or ftype=structured)
# APPFLAGS += -DSTRUCTURED_FUTURES

BASEFLAGS = #-fcilkplus -fcilk-no-inline
REACHFLAGS = -fcilktool
RDFLAGS = -fsanitize=thread -DRACE_DETECT  $(BAGFLAGS) -fno-omit-frame-pointer

ifeq ($(btype),base)
  APPFLAGS = $(BASEFLAGS)
else ifeq ($(btype),reach)
  APPFLAGS = $(BASEFLAGS) $(REACHFLAGS)
else ifeq ($(btype),rd)
  APPFLAGS = $(BASEFLAGS) $(REACHFLAGS) $(RDFLAGS)
else ifeq ($(btype),all)
  btype = all
else ifeq ($(btype),) # default value
  btype = all
else
  $(error "Invalid benchmark type.")
endif


LDFLAGS += $(LIB) -lm -ldl -lpthread -lrt
