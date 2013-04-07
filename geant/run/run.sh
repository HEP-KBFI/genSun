#!/bin/bash
# Arguments: prefix particle energy runs
prefix=$1
particle=$2
energy=$3
runs=$4

ofname=working/${prefix}/${prefix}_${SLURM_PROCID}.root

echo "Run ${SLURM_PROCID} start:" `date` " -- fout:" ${ofname}
time ./../build/solnugeant -q -n${runs} --ofile=${ofname} ${particle} ${energy}
echo "Run {SLURM PROCID} stop:" `date`

#echo procid:$SLURM_PROCID localid:$SLURM_LOCALID
