include ../common.mk

SRC := tsan.cpp shadow_stack.cpp tool.cpp
OBJ := $(SRC:.cpp=.o)
LIB_DIR = $(BUILD_DIR)/lib
TARGET = $(LIB_DIR)/libfuturerd.a

$(TARGET): $(OBJ) $(LIB_DIR)

$(LIB_DIR):
	if [ ! -e $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi

clean:
	rm -rf $(addprefix $(OBJ_DIR)/,$(OBJ)) $(TARGET)
