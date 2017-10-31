include ../../bench.mk

.PHONY: all clean reallyclean

TARGETS = lcs sw

all: $(TARGETS) 

INC += -I../util

sw: sw.o ../util/getoptions.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

lcs: lcs.o ../util/getoptions.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

# delete all object files
clean:
	rm -f *.o $(TARGETS) core.*

reallyclean:
	rm -f ../util/*.o *.o $(TARGETS) core.*
