include ../bench.mk
TARGETS := matmul

LIB = $(LIB_DIR)/librd.a

.SECONDARY:

all: $(TARGETS)
$(TARGETS) : % : %_bench


# -fsanitize-blacklist=blacklist.txt 
# %_bench: $(OBJ_DIR)/%.o $(LIB)
%_bench: %.o getoptions.o $(LIB)
	$(CXX) $< getoptions.o $(LIB) $(LDFLAGS) -o $(@:_bench=)

.PHONY: all clean check %_bench $(TARGETS)

ccyellow=\033[0;33m
ccred=\033[31;01m
ccend=\033[0m

clean:
	rm -rf $(TARGETS) *.o
