BENCH_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(BENCH_DIR)/../common.mk

INC += -I$(PROJECT_HOME)/src    # for future.hpp
INC += -I$(BUILD_DIR)/include   # for runtime include
INC += -I$(BENCH_DIR)           # for util stuff
RDLIB = $(LIB_DIR)/librd-$(ftype).a

BASEFLAGS = -fcilkplus -fcilk-no-inline
REACHFLAGS = -fcilktool -DREACH_MAINT
INSTFLAGS = -fsanitize=thread 
RDFLAGS = -DRACE_DETECT -fno-omit-frame-pointer

ifeq ($(btype),base)
  APPFLAGS = $(BASEFLAGS)
  LIB = $(RUNTIME_LIB)
else ifeq ($(btype),reach)
  APPFLAGS = $(BASEFLAGS) $(REACHFLAGS)
  LIB = $(RDLIB) $(RUNTIME_LIB:.a=-cilktool.a)
else ifeq ($(btype),inst)
  APPFLAGS = $(BASEFLAGS) $(REACHFLAGS) $(INSTFLAGS)
  LIB = $(RDLIB) $(RUNTIME_LIB:.a=-cilktool.a)
else ifeq ($(btype),rd)
  APPFLAGS = $(BASEFLAGS) $(REACHFLAGS) $(INSTFLAGS) $(RDFLAGS)
  LIB = $(RDLIB) $(RUNTIME_LIB:.a=-cilktool.a)
else ifeq ($(btype),all)
  btype = all
else ifeq ($(btype),) # default value
  btype = all
else
  $(error "Invalid benchmark type.")
endif


LDFLAGS += $(LIB) -lm -ldl -lpthread -lrt
