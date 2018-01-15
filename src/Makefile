include ../common.mk

# Need this flag or the race detector won't do anything
CFLAGS += -DRACE_DETECT
CXXFLAGS += -DRACE_DETECT

# Basic tool functionality
SRC := driver.cpp rd.cpp

# Shadow memory
SRC += mem_access.cpp shadow_mem.cpp

# Reachability
SRC += reach_gen.cpp reach_nonblock.cpp # Don't need for structured
SRC += reach_structured.cpp # Even nonblocking needs this

# Other helpers
SRC += utils/union_find.cpp debug.cpp spbag.cpp

OBJ := $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))
LIB_DIR = $(BUILD_DIR)/lib
TARGET = $(LIB_DIR)/librd-$(ftype).a

# Mostly for cilk/internal includes
INC += -I$(PROJECT_HOME)/cilkrts/include

$(TARGET): $(OBJ) $(LIB_DIR) $(SRC)

$(LIB_DIR):
	if [ ! -e $(LIB_DIR) ]; then mkdir -p $(LIB_DIR); fi

clean:
	rm -rf $(OBJ) $(TARGET)
