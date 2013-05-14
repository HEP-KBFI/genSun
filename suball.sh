#!/bin/bash
masses=( 100000 )
particles=( 6 )
timestamp=`date +%F`
OFDIR=/home/joosep/solarNu/spec_out_test_$timestamp
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
#                sbatch -p main $DIR/runSolarSpectrum.sh $p $m 0 0 0 $OFDIR
#                qsub $DIR/runSolarSpectrum.sh $p $m 1 1 1 $OFDIR
#                sbatch -p prio $DIR/runSolarSpectrum.sh $p $m 0 0 0 $OFDIR
#                qsub $DIR/runSolarSpectrum.sh $p $m 1 0 0 $OFDIR
#                qsub $DIR/runSolarSpectrum.sh $p $m 0 1 0 $OFDIR
#                qsub $DIR/runSolarSpectrum.sh $p $m 0 0 1 $OFDIR
#                qsub $DIR/runSolarSpectrum.sh $p $m 2 0 0 $OFDIR
#                qsub $DIR/runSolarSpectrum.sh $p $m 0 0 2 $OFDIR
            done
    done
