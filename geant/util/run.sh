#!/bin/bash
#SBATCH -J "solnuG4" -x comp-c-[002,004,006,009-012,014,016,020,022,024,025,028,031,032,034,036,037],comp-d-[001,083,098,111,117]
export MASTERSEED=66600000000
echo "Base seed:" $MASTERSEED
srun find.sh

#for i in {0..500}; do
#	seed=$(( 19917398179 + $i*1000000 ))
#	seed=$(( 10000000000 + $i*1000000 ))
#	srun find.sh $seed
#	echo $seed
#done
