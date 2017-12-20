# I wrote this Makefile, but I no longer understand how or why it does
# what it does. -Rob
include ../test.mk
SRC := $(wildcard test[0-9].cpp) $(wildcard test[0-9][0-9].cpp)
TARGETS := $(SRC:.cpp=)

LIB = $(LIB_DIR)/librd.a
LDFLAGS += $(LIB)

.SECONDARY:

all: $(TARGETS)
	@echo $(SRC)
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
		printf "$$test ..."; \
		./$$test ; \
		printf " passed!\n"; \
	done
	@printf "$(ccyellow) ... all passed! $(ccend)\n"

clean:
	rm -rf $(TARGETS) *.o core.*
