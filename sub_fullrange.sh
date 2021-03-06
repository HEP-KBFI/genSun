#!/bin/bash

masses=( 5 6 8 10 15 20 25 30 40 50 60 70 80 90 100 110 120 130 140 150 160 180 200 220 240 260 280 300 330 360 400 450 500 550 600 650 700 750 800 900 1000 1100 1200 1300 1500 1700 2000 2500 3000 4000 5000 6000 7000 8000 9000 10000 12000 15000 20000 30000 50000 100000 )

particles=( 1 4 5 6 11 12 13 14 15 21 22 23 24 25 )

timestamp=`date +%F`$TAG

OFDIR=/home/joosep/solarNu/spec_out_fullrange_$timestamp
if [ ! -d $OFDIR ]; then
    mkdir $OFDIR
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $OFDIR
for m in "${masses[@]}"
    do
        for p in "${particles[@]}"
            do
                sleep 0.05
                sbatch -p main $DIR/runSolarSpectrum.sh $p $m 2 1 2 $OFDIR
            done
    done
