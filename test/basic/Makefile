include ../test.mk
TARGETS := test0 test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11

LIB = $(LIB_DIR)/librd.a

.SECONDARY:

all: $(TARGETS)
$(TARGETS) : % : %_test

# -fsanitize-blacklist=blacklist.txt 
# %_test: $(OBJ_DIR)/%.o $(LIB)
%_test: %.o $(LIB)
	$(CXX) $< $(LIB) $(LDFLAGS) -o $(@:_test=)

.PHONY: all clean check %_test $(TARGETS)

ccyellow=\033[0;33m
ccred=\033[31;01m
ccend=\033[0m

check: $(TARGETS)
	@printf "$(ccyellow) Begin basic tests ... $(ccend)\n"
	@for test in $(TARGETS); do \
		./$$test ; \
	done
	@printf "$(ccyellow) ... passed! $(ccend)\n"

clean:
	rm -rf $(TARGETS) *.o
