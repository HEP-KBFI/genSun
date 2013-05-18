#!/bin/bash
uname -a
date
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

cp -R /home/joosep/solarNu/pythia8176 $WD
cd $WD
cd genSun
#LD_LIBRARY_PATH=/home/joosep/local/lib:/home/joosep/local/lib64:$LD_LIBRARY_PATH ./genSun.exe $PARTID $DMMASS output.root cardSunBatch.card $HHADINSTR $LHADINSTR $LEPINSTR &> log \
for i in {1..50}
do
    echo "Trying to run genSun.exe, try "$i
    srun ./genSun.exe $PARTID $DMMASS output.root cardSunBatch.card $HHADINSTR $LHADINSTR $LEPINSTR
    if [ $? -eq 0 ]
    then
        break
    fi
done
cp output.root $OFDIR/output_$SLURM_JOB_ID.root
#mv log $OFDIR/log_$SLURM_JOB_ID.txt

ls -al
rm -Rf $WD
