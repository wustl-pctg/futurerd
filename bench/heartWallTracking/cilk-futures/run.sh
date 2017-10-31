#!/bin/sh
# The first param is the number of frames to process, 
# second is num of omp threads
# original input: 20 4
# full input: 104 1
time ./heartwall ../data/test.avi 10 1
