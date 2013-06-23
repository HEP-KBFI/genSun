#!/bin/bash
# Arguments: prefix particle energy runs physics
prefix=$1
shift 1

ofname=working/${prefix}/${prefix}_${SLURM_PROCID}.root

seed=$(($BASESEED + 100*$SLURM_PROCID))

echo "Run ${SLURM_PROCID} start:" `date`
echo "ofname:" ${ofname}
echo "arguments: $@"

retval=134
while [ $retval == 134 ]; do
	echo "Seed:" $seed "("$BASESEED")"
	time ./../install/solnugeant -q --ofile=${ofname} --seed=$seed $@
	retval=$?; echo "Return value:" $retval
	seed=$(($seed + 1))
done

echo "Run ${SLURM_PROCID} stop:" `date`

#echo procid:$SLURM_PROCID localid:$SLURM_LOCALID
