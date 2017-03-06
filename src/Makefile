include ../common.mk

# Basic tool functionality
SRC := shadow_stack.cpp driver.cpp futurerd.cpp sp_tsan.cpp

# Reachability
SRC += sp_reach.cpp reach.cpp

# Other helpers
SRC += om/two_list.cpp utils/union_find.cpp debug.cpp

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
