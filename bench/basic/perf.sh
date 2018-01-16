#!/bin/bash
set -e

# n=1024

# for t in base reach rd; do
#     for b in sw lcs matmul_z; do
#         perf record --call-graph dwarf -- ./$b-$t -n $n;
#         #perf annotate --stdio > perf/$b-$t.anno
#         mv perf.data perf/$b-$t.data;
#     done
# done

make mode=profile ftype=structured btype=base perf
perf record -g ./perf-base
perf annotate --stdio 2>/dev/null > perf.anno
grep "main" perf.anno
