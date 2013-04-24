#!/bin/bash
#SBATCH -J "solnuG4"

# Parse arguments
GETOPT=$(getopt -n "batch" -o r:p: -- "$@")
runs=1 # default number of runs
physics="FULL" # default physics
eval set -- "$GETOPT"
while true; do
	case $1 in
		-r)
			runs=$2
			echo "Number of runs:" $runs
			shift 2; continue
		;;
		-p)
			physics=$2
			echo "Physics:" $physics
			shift 2; continue
		;;
		--)
			break
		;;
		*)
			echo "Unknown option" $1
			exit 1
		;;
	esac
done
eval set -- "$@"; shift

# Check for directories
if [ ! -d working ]; then mkdir working; fi
if [ ! -d ready ]; then mkdir ready; fi

particle=$1
energy=$2

prefix=hist_p${particle}_e${energy}_job${SLURM_JOB_ID}

echo "Starting batch job" ${SLURM_JOB_ID} "on" ${SLURM_JOB_NUM_NODES} "nodes"
echo "Using prefix:" $prefix
date

mkdir working/$prefix

echo "Starting Geant4 runs"
srun -l ./run.sh ${prefix} ${particle} ${energy} ${runs} ${physics}
echo "Geant simulations done"

echo "hadding histograms"
hadd ready/$prefix.root working/$prefix/*

echo "Batch job" ${SLURM_JOB_ID} "done!"
date
