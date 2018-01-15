THIS_FILE := $(lastword $(MAKEFILE_LIST))
include ../bench.mk

BENCH = lcs sw matmul_z #perf
ALLTARGETS = $(BENCH:=-rd) $(BENCH:=-reach) $(BENCH:=-base)
TARGETS = $(BENCH:=-$(btype))

.PHONY: all clean $(BENCH) $(BENCH:=-all)
all: $(TARGETS)

$(BENCH:=-all):
	$(MAKE) -f $(THIS_FILE) mode=$(mode) ftype=$(ftype) btype=rd $(@:-all=)
	$(MAKE) mode=$(mode) ftype=$(ftype) btype=reach $(@:-all=)
	$(MAKE) mode=$(mode) ftype=$(ftype) btype=base $(@:-all=)

INC += -I../util
CFLAGS += $(APPFLAGS)
CXXFLAGS += $(APPFLAGS)

.SECONDEXPANSION:
$(BENCH): $$@-$(btype)
$(ALLTARGETS): $$@.o ../util/getoptions.o $(RDLIB)
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

ifeq ($(btype),all)
clean:
	rm -rf core.* $(ALLTARGETS) $(ALLTARGETS:=.o)
else
clean:
	rm -f core.* $(TARGETS) $(TARGETS:=.o)
endif
