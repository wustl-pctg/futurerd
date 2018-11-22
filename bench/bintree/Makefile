include ../bench.mk

.PHONY: all clean reallyclean

TARGETS = merge-$(btype)

all: $(TARGETS)

INC += -I../util
CFLAGS += $(APPFLAGS)
CXXFLAGS += $(APPFLAGS)

merge-all:
	$(MAKE) mode=$(mode) rdalg=$(rdalg) ftype=$(ftype) btype=base
	$(MAKE) mode=$(mode) rdalg=$(rdalg) ftype=$(ftype) btype=reach
	$(MAKE) mode=$(mode) rdalg=$(rdalg) ftype=$(ftype) btype=inst
	$(MAKE) mode=$(mode) rdalg=$(rdalg) ftype=$(ftype) btype=rd

merge-%.o: merge.cpp
bintree-%.o: bintree.cpp

# There seems to be a very strange error when using both LTO and -O1
# optimizations. It causes a segfault during get_options, but I cannot
# find anything that is really a problem. Interesting, if you include
# getoptions.cpp instead of getoptions.hpp, the problem goes away, so
# the process of linking is somehow involved (bug in gold?). Anyway,
# the code is not on the critical path, so just don't use
# optimizations for this file.
../util/getoptions.o: ../util/getoptions.cpp
	$(CXX) -O0 $(CLAGS) -O0 -o $@ -c $<

merge-%: merge-%.o bintree-%.o ../util/getoptions.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

.SECONDARY:

# delete all object files
clean:
	rm -f *.o $(TARGETS) core.* merge-* ../util/*.o

reallyclean:
	rm -f *.o merge-base merge-reach merge-inst merge-rd ../util/*.o core.*
