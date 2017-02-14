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

ccyellow=$(shell echo -e "\033[0;33m")
ccend=$(shell echo -e "\033[0m")

check: $(TARGETS)
	$(ccyellow); @echo "---------- Begin basic tests ----------"; $(ccend)
	@for test in $(TARGETS); do \
		./$$test ; \
	done
	$(ccyellow); @echo "---------- Passed basic tests ----------"; $(ccend)

clean:
	rm -rf $(TARGETS) *.o
