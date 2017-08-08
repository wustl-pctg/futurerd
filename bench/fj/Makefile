TARGETS = matmul heat fft cholesky cilksort fib fibx strassen
TARGETS += knapsack lu nqueens qsort rectmul #matmul-int
OBJ_DIR_PREFIX := obj
BIN_DIR := bin
MODES = rd base bags # stats prof

ifeq ($(mode), rd)
	OBJ_DIR := $(OBJ_DIR_PREFIX:=_rd)
	CFLAGS += $(RDFLAGS)
	CXXFLAGS += $(RDFLAGS)
	LIB = $(LIB_DIR)/lib$(TOOL_NAME).a
else ifeq ($(mode), base)
	OBJ_DIR := $(OBJ_DIR_PREFIX:=_base)
else ifeq ($(mode), bags)
	OBJ_DIR := $(OBJ_DIR_PREFIX:=_bags)
	CFLAGS += $(INSERTFLAGS) 
	CXXFLAGS += $(INSERTFLAGS)
	LIB = $(LIB_DIR)/lib$(TOOL_NAME).a
else
	CFLAGS += $(INSERTFLAGS) 
	CXXFLAGS += $(INSERTFLAGS)
endif
endif
include ../bench.mk

most: rd base bags
all: most #stats prof
rd:
	$(MAKE) $(TARGETS) mode=rd
base:
	$(MAKE) $(TARGETS) mode=base
bags:
	$(MAKE) $(TARGETS) mode=bags

$(BIN_DIR):
	mkdir $(BIN_DIR)

ifdef mode
fib: $(OBJ_DIR)/fib.o $(OBJ_DIR)/options.o
fibx: $(OBJ_DIR)/fibx.o $(OBJ_DIR)/options.o
qsort: $(OBJ_DIR)/qsort.o $(OBJ_DIR)/options.o
nqueens: $(OBJ_DIR)/nqueens.o $(OBJ_DIR)/options.o
matmul: $(OBJ_DIR)/matmul.o $(OBJ_DIR)/options.o
heat: $(OBJ_DIR)/heat.o $(OBJ_DIR)/options.o
fft: $(OBJ_DIR)/fft.o $(OBJ_DIR)/options.o
cholesky: $(OBJ_DIR)/cholesky.o $(OBJ_DIR)/options.o
cilksort: $(OBJ_DIR)/cilksort.o $(OBJ_DIR)/options.o
strassen: $(OBJ_DIR)/strassen.o $(OBJ_DIR)/options.o
knapsack: $(OBJ_DIR)/knapsack.o $(OBJ_DIR)/options.o
lu: $(OBJ_DIR)/lu.o $(OBJ_DIR)/options.o
rectmul: $(OBJ_DIR)/rectmul.o $(OBJ_DIR)/options.o
test: $(OBJ_DIR)/test.o

%_base:
	$(MAKE) $(@:_base=) mode=base
%_rd:
	$(MAKE) $(@:_brd=) mode=rd
%_bags:
	$(MAKE) $(@:_insert=) mode=bags

%_all:
	$(MAKE) $(@:_all=_base)
	$(MAKE) $(@:_all=_bags)
	$(MAKE) $(@:_all=_rd)

.PHONY: clean modeclean %_clean $(TARGETS) %_rd %_base %_bags

%_clean:
	rm -rf $(BIN_DIR)/$(@:_clean=)_*
	for m in $(MODES); do \
		rm -f $(OBJ_DIR_PREFIX)_$$m/$(@:_clean=.o); \
	done

clean:
	$(MAKE) modeclean mode=base
	$(MAKE) modeclean mode=brd
	$(MAKE) modeclean mode=insert

modeclean:
	rm -rf *.o $(OBJ_DIR) $(BIN_DIR)/*_$(mode)
