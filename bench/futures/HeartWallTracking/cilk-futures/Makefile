include ../../../bench.mk

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++

RTS_INCLUDE=$(BUILD_DIR)/include
LIBS=-lcilkrts -ldl
LIB_PATH=-L$(BUILD_DIR)/lib -Wl,-rpath -Wl,$(BUILD_DIR)/lib

DEFINES = -DOUTPUT # -DRACE_DETECT

OPT = -O3 -flto
INCLUDE=-I$(PROJECT_HOME)/src
CFLAGS = -g -Werror -Wall $(DEFINES) -std=c99 $(OPT) -fcilkplus $(INCLUDE)
CXXFLAGS = -g -Werror -Wall $(DEFINES) -std=c++11 $(OPT) -fcilkplus $(INCLUDE)

all: heartwall

compute-steps.o: compute-steps.cpp
	$(CXX) $< -I./AVI -I$(RTS_INCLUDE) -c $(CXXFLAGS)

# link objects(binaries) together
heartwall: main.o compute-steps.o ./AVI/avilib.o ./AVI/avimod.o
	$(CXX) $^ $(OPT) $(LIB_PATH) -lm $(LIBS) -o heartwall

# compile main function file into object (binary)
main.o: main.cpp define.hpp
	$(CXX) $< -I./AVI -I$(RTS_INCLUDE) -c $(CXXFLAGS)

./AVI/avilib.o ./AVI/avimod.o:
	cd AVI; make;

# delete all object files
clean:
	rm -f *.o AVI/*.o heartwall result.txt
