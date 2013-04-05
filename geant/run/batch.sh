#!/bin/bash
particle=$1
energy=$2

prefix=hist_p${particle}_e${energy}_job${SLURM_JOB_ID}

echo "Starting batch job" ${SLURM_JOB_ID} "on" ${SLURM_JOB_NUM_NODES} "nodes..."
#srun -l ./run.sh ${prefix} 22 1000 # gamma at 1000 GeV
srun -l ./run.sh ${prefix} ${particle} ${energy} # gamma at 1000 GeV
echo "Batch job" ${SLURM_JOB_ID} "done!"
