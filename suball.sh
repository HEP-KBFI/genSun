#!/bin/bash
#masses=( 1000 2500 5000 7500 10000 )
masses=( 5000 )
particles=( 1 5 6 11 12 13 15 22 23 24 25 )
#particles=( 6 15 )
OFDIR=/home/joosep/solarNu/out
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
for m in "${masses[@]}"
    do
        for p in "${particles[@]}"
            do
                qsub $DIR/runSolarSpectrum.sh $p $m 2 1 2 $OFDIR
                qsub $DIR/runSolarSpectrum.sh $p $m 1 1 1 $OFDIR
                qsub $DIR/runSolarSpectrum.sh $p $m 0 0 0 $OFDIR
                qsub $DIR/runSolarSpectrum.sh $p $m 1 0 0 $OFDIR
                qsub $DIR/runSolarSpectrum.sh $p $m 0 1 0 $OFDIR
                qsub $DIR/runSolarSpectrum.sh $p $m 0 0 1 $OFDIR
                qsub $DIR/runSolarSpectrum.sh $p $m 2 0 0 $OFDIR
                qsub $DIR/runSolarSpectrum.sh $p $m 0 0 2 $OFDIR
            done
    done
