#!/bin/bash
set -e

PROGS=(lcs sw matmul_z hw dedup)
source remake.sh
system release

allbench release structured structured
source ./time.sh
mv times.csv times.ss.csv

allbench release nonblock structured
source ./time.sh
mv times.csv times.ns.csv

PROGS=(lcs sw matmul_z hw dedup merge)
allbench release nonblock nonblock
source ./time.sh
mv times.csv times.nn.csv
