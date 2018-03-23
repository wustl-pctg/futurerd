#!/bin/bash

# curr=`pwd`
bindir='./build'
datadir='./data'

if [ $# -le 1 ]; then
printf "Usage: ./run.sh <prog> <data size> [nproc]\n"
printf "where prog includes: serial, pthread, omp, cilk\n"
printf "      and data size includes: simdev, simsmall, simmedium, simlarge, native.\n"
printf "[nproc] by default is set to 1 if not specified.\n"
exit 0
fi

prog=$1
dsize=$2
nproc="1"
if [ $# -ge 3 ]; then 
    nproc=$3
fi
outputBMP=1

case "$prog" in 
    "serial") model=0
    ;; 
    "pthread") model=2
    ;; 
    "omp") model=3
    ;; 
    "cilk") model=4
    ;; 
esac

# args = <data path> <# camera> <# frames> <# particles> <annealing layers>
# <thread model> <nproc> <output>
case "$dsize" in
    "simdev")    
        dpath="$datadir/$dsize/sequenceB_1"
        args="$dpath 4 1 100 3 $model $nproc $outputBMP"
    ;;
    "simsmall")  
        dpath="$datadir/$dsize/sequenceB_1"
        args="$dpath 4 1 1000 5 $model $nproc $outputBMP"
    ;;
    "simmedium") 
        dpath="$datadir/$dsize/sequenceB_2"
        args="$dpath 4 2 2000 5 $model $nproc $outputBMP"
    ;;
    "simlarge")  
        dpath="$datadir/$dsize/sequenceB_4"
        args="$dpath 4 4 4000 5 $model $nproc $outputBMP"
    ;;
    "native")    
        dpath="$datadir/$dsize/sequenceB_261"
        args="$dpath 4 261 4000 5 $model $nproc $outputBMP"
    ;;
esac

cmd="$bindir/bodytrack-$prog $args"

printf "$cmd\n"
$cmd

printf "\nChecking results ... \n"
printf "diff $dpath/poses.txt $dpath/correct-poses.txt\n"
diff $dpath/poses.txt $dpath/correct-poses.txt
