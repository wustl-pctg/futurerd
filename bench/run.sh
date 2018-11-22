#!/bin/bash
set -e

PROGS=(lcs sw matmul_z hw dedup merge)
source remake.sh
system release

allbench release structured structured
bintree release structured structured
source ./time.sh
mv times.csv times.ss.csv

allbench release nonblock structured
bintree release nonblock structured
source ./time.sh
mv times.csv times.ns.csv

allbench release nonblock nonblock
bintree release nonblock nonblock
source ./time.sh
mv times.csv times.nn.csv
