include ../bench.mk

.PHONY: all clean reallyclean

TARGETS = lcs sw matmul_z

all: $(TARGETS) 

INC += -I../util
# CXXFLAGS += -fsanitize=undefined
# LIB += $(LIB_DIR)/libubsan.a

lcs: lcs.o ../util/getoptions.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

sw: sw.o ../util/getoptions.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

matmul_z: matmul_z.o ../util/getoptions.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

# delete all object files
clean:
	rm -f *.o $(TARGETS) core.*

reallyclean:
	rm -f ../util/*.o *.o $(TARGETS) core.*
