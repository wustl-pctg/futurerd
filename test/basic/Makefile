include ../test.mk
TARGETS := test1

LIB = $(LIB_DIR)/libfuturerd.a

all: $(TARGETS)
$(TARGETS) : % : %_test

# %_test: $(OBJ_DIR)/%.o $(LIB)
%_test: %.o $(LIB)
	$(CXX) $< $(LIB) $(LDFLAGS) -o $(@:_test=)

.PHONY: all clean %_test $(TARGETS)

clean:
	rm -rf $(TARGETS) *.o
