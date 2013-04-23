#!/bin/bash
uname -a
WD=/scratch/joosep/solarNuWD-$SLURM_JOB_ID
source /home/joosep/solarNu/setenv.sh

PARTID=$1
DMMASS=$2
HHADINSTR=$3
LHADINSTR=$4
LEPINSTR=$5
OFDIR=$6

echo "PARTID="$PARTID
echo "DMMASS="$DMMASS
echo "HHADINSTR="$HHADINSTR
echo "LHADINSTR="$LHADINSTR
echo "LEPINSTR="$LEPINSTR

cp -R /home/joosep/solarNu/pythia8175 $WD
cd $WD
cd genSun
./genSun.exe $PARTID $DMMASS output.root cardSunBatch.card $HHADINSTR $LHADINSTR $LEPINSTR \
&& cp output.root $OFDIR/output_$SLURM_JOB_ID.root

ls -al
rm -Rf $WD
