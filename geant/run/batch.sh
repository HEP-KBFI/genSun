#!/bin/bash
#SBATCH -J "solnuG4" -x comp-c-[002,004,022,028,031,032,034,037,038]

# Parse arguments
GETOPT=$(getopt -n "batch" -o r:p:u:c: -- "$@")
runs=1 # default number of runs
physics="FULL" # default physics
units="G" # default GeV units
creator="PYTHIA8"
creator_suffix=""
eval set -- "$GETOPT"
while true; do
	case $1 in
		-r)
			runs=$2
			shift 2; continue
		;;
		-p)
			physics=$2
			shift 2; continue
		;;
		-u)
			units=$2
			shift 2; continue
		;;
		-c)
			creator=$2
			creator_suffix="_G4"
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

prefix=hist_job${SLURM_JOB_ID}_p${particle}_e${energy}${units}_P${physics}${creator_suffix}

echo "Starting batch job" ${SLURM_JOB_ID} "on" ${SLURM_JOB_NUM_NODES} "nodes"
date
echo "Prefix:" $prefix

echo "date="`date`
echo "runs="$runs
echo "physics="$physics
echo "particle="$particle
echo "energy="$energy
echo "prefix="$prefix
echo "creator="$creator

echo "Nodelist:" ${SLURM_NODELIST}
echo "Tasks per node:" ${SLURM_TASKS_PER_NODE}

mkdir working/$prefix

echo "Starting Geant4 runs"
srun -l ./run.sh ${prefix} -n${runs} ${particle} ${energy} -u${units} -p${physics} -c${creator}
echo "Geant simulations done"

echo "hadding histograms"
hadd ready/$prefix.root working/$prefix/*

echo "Batch job" ${SLURM_JOB_ID} "done!"
date
