#!/bin/bash
#SBATCH -J "solnuG4" -x comp-c-[002,004,006,009-012,014,016,020,022,024,025,028,031,032,034,036,037],comp-d-[001,083,098,111,117]

# Parse arguments
GETOPT=$(getopt -n "batch" -o r:p:u:c: -l trk: -- "$@")
runs=1 # default number of runs
physics="FULL" # default physics
units="G" # default GeV units
creator="PYTHIA8"
creator_suffix=""
trk_arg=""
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
		--trk)
			trk_arg="--track-kill="$2
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

# Positional arguments
particle=$1
energy=$2
shift 2

# Check for output directory
if [ ! -d hists ]; then mkdir hists; fi

prefix=hist_job${SLURM_JOB_ID}_p${particle}_e${energy}${units}_P${physics}${creator_suffix}

echo "Starting batch job" ${SLURM_JOB_ID}
date

echo "prefix="$prefix
echo "date="`date`
echo "runs="$runs
echo "physics="$physics
echo "particle="$particle
echo "energy="$energy
echo "prefix="$prefix
echo "creator="$creator

seed=$(($(($(date +%s) + $RANDOM)) + ${SLURM_JOB_ID}))
echo "seed="$seed

ofname=hists/${prefix}.root
echo "ofname="$ofname

echo "Run ${SLURM_PROCID} start:" `date`
echo "arguments=$@"

echo "Starting SolNuGeant (Geant4) simulation.."
time ./../solnugeant -q --ofile=${ofname} --seed=$seed -n${runs} ${particle} ${energy} -u${units} -p${physics} -c${creator} ${trk_arg} $@
echo "Geant simulation finished!"

echo "Batch job" ${SLURM_JOB_ID} "done!"
date
