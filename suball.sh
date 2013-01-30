#!/bin/bash
masses=( 1000 2500 5000 7500 10000 )
particles=( 1 5 6 11 12 13 15 22 23 24 25 )

for m in "${masses[@]}"
    do
        for p in "${particles[@]}"
            do
                qsub sub.sh $p $m 2 1 2
                qsub sub.sh $p $m 1 1 1
                qsub sub.sh $p $m 0 0 0
            done
    done
