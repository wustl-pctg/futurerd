include ../../bench.mk

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++

RTS_INCLUDE=$(BUILD_DIR)/include
LIBS=-lcilkrts -ldl
LIB_PATH=-L$(BUILD_DIR)/lib -Wl,-rpath -Wl,$(BUILD_DIR)/lib

all: sw 

DEFINES = # -DRACE_DETECT

OPT = -O3
CFLAGS = -g -Werror -Wall $(DEFINES) -std=c99 $(OPT) -fcilkplus $(INCLUDE)
CXXFLAGS = -g -Werror -Wall $(DEFINES) -std=c++11 $(OPT) -fcilkplus $(INCLUDE)
 
sw: sw.o getoptions.o
	$(CXX) $^ $(OPT) $(LIB_PATH) -lm $(LIBS) -o sw 

%.o: %.cpp
	$(CXX) $< -I$(RTS_INCLUDE) -c $(CXXFLAGS)

# delete all object files
clean:
	rm -f *.o sw core.*
