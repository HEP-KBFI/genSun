#!/bin/bash
# Arguments: prefix particle energy runs physics
prefix=$1
shift 1

ofname=working/${prefix}/${prefix}_${SLURM_PROCID}.root

echo "Run ${SLURM_PROCID} start:" `date`
echo "ofname:" ${ofname}
echo "arguments: $@"
time ./../build/solnugeant -q --ofile=${ofname} $@
echo "Run ${SLURM_PROCID} stop:" `date`

#echo procid:$SLURM_PROCID localid:$SLURM_LOCALID
