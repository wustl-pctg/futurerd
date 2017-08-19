include ../../bench.mk

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++

RTS_INCLUDE=$(BUILD_DIR)/include
LIBS=-lcilkrts -ldl
LIB_PATH=-L$(BUILD_DIR)/lib -Wl,-rpath -Wl,$(BUILD_DIR)/lib

all: lcs

DEFINES = # -DRACE_DETECT

OPT = -O3 -flto
INCLUDE=-I$(PROJECT_HOME)/src
CFLAGS = -g -Werror -Wall $(DEFINES) -std=c99 $(OPT) -fcilkplus $(INCLUDE)
CXXFLAGS = -g -Werror -Wall $(DEFINES) -std=c++11 $(OPT) -fcilkplus $(INCLUDE)
 
lcs: lcs.o getoptions.cpp
	$(CXX) $^ $(OPT) $(LIB_PATH) -lm $(LIBS) -o lcs

%.o: %.cpp
	$(CXX) $< -I$(RTS_INCLUDE) -c $(CXXFLAGS)

# delete all object files
clean:
	rm -f *.o lcs core.*
