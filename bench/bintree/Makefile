include ../bench.mk

.PHONY: all clean reallyclean

TARGETS = merge

all: $(TARGETS) 

INC += -I../util
CFLAGS += $(APPFLAGS) #$(RDFLAGS)
CXXFLAGS += $(APPFLAGS) #$(RDFLAGS)

merge: merge.o bintree.o ../util/getoptions.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

# delete all object files
clean:
	rm -f *.o $(TARGETS) core.*

reallyclean:
	rm -f *.o $(TARGETS) ../util/*.o core.*
