include ../test.mk
TARGETS := test1

# $(LIB_DIR)/libfuturerd.a
LIB =

all: $(TARGETS)
$(TARGETS) : % : %_test


%_test: CFLAGS += $(RDFLAGS)
%_test: CXXFLAGS += $(RDFLAGS)
%_test: LDFLAGS += $(RUNTIME_LIB)
%_test: $(OBJ_DIR)/%.o $(LIB)
	$(CXX) $(OBJ_DIR)/$< $(LIB) $(LDFLAGS) -o $(@:_test=)

.PHONY: all clean %_test $(TARGETS)

clean:
	rm -rf $(TARGETS) *.o
