include ../../../../bench.mk

CC = $(COMPILER_HOME)/bin/clang
CXX = $(COMPILER_HOME)/bin/clang++

all: avimod.o avilib.o

OPT = -O3
CFLAGS = -g -Werror -Wall $(OUTPUT) -std=c99 $(OPT) 
CXXFLAGS = -g -Werror -Wall $(OUTPUT) -std=c++11 $(OPT)  

# compile supporting function file into object (binary)
avimod.o: avilib.hpp avimod.cpp
	$(CXX) -c $(CXXLFAGS) avimod.cpp

# compile supporting function file into object (binary)
avilib.o: avilib.hpp avilib.cpp
	$(CXX) -c $(CXXFLAGS) avilib.cpp

# delete files
clean:
	rm *.o *.out
