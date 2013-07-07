#!/bin/bash
seed=seed=$(($MASTERSEED + 1000000*$SLURM_PROCID))

while [ TRUE ]; do
	echo "SEED:" $seed `date`
	./solnugeant -q -n10 13 10 --seed=$seed -o output.$seed.root &> find/find.$seed.out.tmp
	rv=$?
	
	rm output.$seed.root
	
	if [ $rv != 0 ]
	then
		echo $seed "RV:"$rv
		echo $seed "RV:"$rv >> find/find.out
		mv find/find.$seed.out.tmp find/find.$seed.out
	else
		rm find/find.$seed.out.tmp
	fi
	
	seed=$(($seed+1))
done
