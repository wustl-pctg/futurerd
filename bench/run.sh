#!/bin/bash
set -e

# remake structured
# basecases
# mv basic/bc.out.log ./bc.structured.log
# mv basic/bc.times.csv ./bc.structured.csv
# printf -- "structured--------------------------------------------------------\n"
# cat bc.structured.csv

#remake nonblock
# basecases
# mv basic/bc.out.log ./bc.nonblock.log
# mv basic/bc.times.csv ./bc.nonblock.csv
# printf -- "nonblock----------------------------------------------------------\n"
# cat bc.nonblock.csv

PROGS=(dedup)
source remake.sh

#system release nonblock
#basic release nonblock structured
source ./time.sh
