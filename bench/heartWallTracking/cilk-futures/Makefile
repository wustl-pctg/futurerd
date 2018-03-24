include ../../bench.mk

DEFINES = -DOUTPUT
CFLAGS += $(APPFLAGS) $(DEFINES) $(INC) -I./AVI
CXXFLAGS += $(APPFLAGS) $(DEFINES) $(INC) -I./AVI

TARGETS = hw-$(btype)

.PHONY: all clean hw-all
all: $(TARGETS)

hw-all:
	$(MAKE) mode=$(mode) ftype=$(ftype) btype=rd
	$(MAKE) mode=$(mode) ftype=$(ftype) btype=inst
	$(MAKE) mode=$(mode) ftype=$(ftype) btype=reach
	$(MAKE) mode=$(mode) ftype=$(ftype) btype=base

hw-%: main-%.o compute-steps-%.o ./AVI/avilib.o ./AVI/avimod.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o $@

.SECONDARY: main-$(btype).o compute-steps-$(btype).o

main-%.o: main.cpp define.hpp
	$(CXX) $(CXXFLAGS) -I./AVI $(INC) -o $@ -c $<

compute-steps-%.o: compute-steps.cpp
	$(CXX) $(CXXFLAGS) -I./AVI $(INC) -o $@ -c $<

./AVI/avilib.o ./AVI/avimod.o:
	cd AVI; make;

# delete all object files
clean:
	rm -f *.o AVI/*.o result.txt
	rm -f hw-base hw-reach hw-inst hw-rd
