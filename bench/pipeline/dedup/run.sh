#!/bin/bash
curr=`pwd`
datadir=$curr/data
rundir=$curr/build

if [ $# -le 1 ]; then
echo "Usage ./run.sh <prog> <data size> [output file] where"
echo "      prog includes: serial, base, reach, inst, rd"
echo "      data size includes simdev, simsmall, simmedium, simlarge, and native."
exit 0
fi

dsize=$2
outfile=$3

if [[ $3 = "" ]]; then
    outfile="dedup.out"
fi

# if [ $1 = 'serial' ]; then
#     cmd="$rundir/dedup-serial "  
# elif [ $1 = 'cilk' ]; then
#     cmd="$rundir/dedup-base "
# fi

cmd="$rundir/dedup-$1"
cmd+=" -c -i $datadir/$dsize/media.dat -o $outfile"

echo "$cmd"
echo `$cmd`

diffcmd="diff $outfile $datadir/$dsize/dedup.out"
echo "$diffcmd"
echo `$diffcmd`

