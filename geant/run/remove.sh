#!/bin/bash
echo "Removing job" $1
rm -v slurm-${1}.out
rm -vR {ready,working}/hist_job${1}_*
