include ../common.mk

# Basic tool functionality
SRC := driver.cpp futurerd.cpp shadow_mem.cpp #mem_access.cpp

# Other helpers
SRC += utils/union_find.cpp debug.cpp sbpag.cpp

OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
LIB_DIR = $(BUILD_DIR)/lib
TARGET = $(LIB_DIR)/libfuturerd.a

# Mostly for cilk/internal includes
INC += -I$(PROJECT_HOME)/cilkrts/include

$(TARGET): $(OBJ) $(LIB_DIR) $(SRC)

$(LIB_DIR):
	if [ ! -e $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi

clean:
	rm -rf $(OBJ) $(TARGET)
