include ../../bench.mk

DEFINES = -DOUTPUT # -DRACE_DETECT
CFLAGS += $(APPFLAGS) $(RDFLAGS) $(DEFINES)
CXXFLAGS += $(APPFLAGS) $(RDFLAGS) $(DEFINES)

all: heartwall

compute-steps.o: compute-steps.cpp
	$(CXX) $< -I./AVI $(INC) -c $(CXXFLAGS) -o $@

# link objects(binaries) together
heartwall: main.o compute-steps.o ./AVI/avilib.o ./AVI/avimod.o
	$(CXX) $^ $(LDFLAGS) $(LIB) -o heartwall

# compile main function file into object (binary)
main.o: main.cpp define.hpp
	$(CXX) $< -I./AVI $(INC) -c $(CXXFLAGS) -o $@

./AVI/avilib.o ./AVI/avimod.o:
	cd AVI; make;

# delete all object files
clean:
	rm -f *.o AVI/*.o heartwall result.txt
