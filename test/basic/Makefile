include ../test.mk
TARGETS := test1 test2 test3 test4 test5 test6

LIB = $(LIB_DIR)/libfuturerd.a

.SECONDARY:

all: $(TARGETS)
$(TARGETS) : % : %_test

# %_test: $(OBJ_DIR)/%.o $(LIB)
%_test: %.o $(LIB)
	$(CXX) $< $(LIB) $(LDFLAGS) -o $(@:_test=)

.PHONY: all clean check %_test $(TARGETS)

check: $(TARGETS)
	for test in $(TARGETS); do \
		./$$test ; \
	done
	@echo "Basic tests passed."

clean:
	rm -rf $(TARGETS) *.o
