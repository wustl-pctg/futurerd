include ../bench.mk

# Some targets I want:
# pipeline -- generates benchmarks from "Pipelining with Futures"
#   ? - merging algorithm on trees
#   ? - insert/delete from treaps
#   ? - insert into 2-6 trees
# ddt -- generates benchmarks from "Data-Driven Tasks and their Implementation"
#   denoise -- Rician Denoising
#   cholesky -- Cholesky Decomposition
#   scholes -- black scholes
#   heartwall -- heart-wall tracking
# cnc -- generates benchmarks from Intel's CnC

PIPELINE = treemerge treapmeld treapdiff 26treeinsert.cpp
DDT = blackscholes cholesky #denoise heartwall
CNC =
OTHER = simple
ALL = $(PIPELINE) $(DDT) $(CNC) $(OTHER)

# "Pipelining with futures" also mentions a few previous things:
# Paul, Vishkin, Wagener: parallel 2-3 trees
# Cole: O(lg n) sorting
# Isn't there a pipelined quicksort out there somewhere?

all: treemerge

treemerge: treemerge.o ds/bintree.o
	$(CXX) $^ $(LDFLAGS) -o $@

treemerge.o: treemerge.cpp ds/bintree.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(ALL)
