#!/bin/bash
# Arguments: prefix particle energy runs physics
prefix=$1
particle=$2
energy=$3
runs=$4
physics=$5

ofname=working/${prefix}/${prefix}_${SLURM_PROCID}.root

echo "Run ${SLURM_PROCID} start:" `date` " -- fout:" ${ofname}
time ./../build/solnugeant -q -n${runs} -p${physics} --ofile=${ofname} ${particle} ${energy}
echo "Run ${SLURM PROCID} stop:" `date`

#echo procid:$SLURM_PROCID localid:$SLURM_LOCALID
