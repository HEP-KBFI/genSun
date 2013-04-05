#!/bin/bash
# Arguments: prefix particle energy runs
prefix=$1
particle=$2
energy=$3
runs=$4

ofname=working/${prefix}/${prefix}_${SLURM_PROCID}.root
echo "Saving to: " ${ofname}

time ./../build/solnugeant -q -n${runs} --ofile=${ofname} ${particle} ${energy}

#echo procid:$SLURM_PROCID localid:$SLURM_LOCALID
