include ../common.mk

SRC := tsan.cpp tool.cpp debug.cpp futurerd.cpp unionfind.cpp
OBJ := $(SRC:.cpp=.o)
LIB_DIR = $(BUILD_DIR)/lib
TARGET = $(LIB_DIR)/libfuturerd.a

# Mostly for cilk/internal includes
INC += -I$(PROJECT_HOME)/cilkrts/include

$(TARGET): $(OBJ) $(LIB_DIR) $(SRC)

$(LIB_DIR):
	if [ ! -e $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi

clean:
	rm -rf $(addprefix $(OBJ_DIR)/,$(OBJ)) $(TARGET)
	rm *.o
