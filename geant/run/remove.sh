#!/bin/bash
echo "Removing job(s)" $@
for x in $@
do
	echo "Removing job" $x
	rm -v slurm-${x}.out
	rm -vR {ready,working}/hist_job${x}_*
done
