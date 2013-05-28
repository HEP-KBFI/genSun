#!/bin/bash
# Arguments: prefix particle energy runs physics
prefix=$1
shift 1

ofname=working/${prefix}/${prefix}_${SLURM_PROCID}.root

seed=$(($BASESEED + 100*$SLURM_PROCID))

echo "Run ${SLURM_PROCID} start:" `date`
echo "ofname:" ${ofname}
echo "arguments: $@"
echo "Seed:" $seed "("$BASESEED")"
time ./../build/solnugeant -q --ofile=${ofname} --seed=$SEED $@
echo "Run ${SLURM_PROCID} stop:" `date`

#echo procid:$SLURM_PROCID localid:$SLURM_LOCALID
