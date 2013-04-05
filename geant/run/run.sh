#!/bin/bash
# Arguments: prefix particle energy
prefix=$1
particle=$2
energy=$3

ofname=${prefix}_${SLURM_PROCID}.root
echo "Saving to: " ${ofname}

time ./../build/solnugeant -q -n200 --ofile=${ofname} ${particle} ${energy}

#echo procid:$SLURM_PROCID localid:$SLURM_LOCALID
