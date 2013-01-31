#!/bin/bash
uname -a
WD=/scratch/joosep/solarNuWD-$SLURM_JOB_ID

PARTID=$1
DMMASS=$2
HHADINSTR=$3
LHADINSTR=$4
LEPINSTR=$5

echo "PARTID="$PARTID
echo "DMMASS="$DMMASS
echo "HHADINSTR="$HHADINSTR
echo "LHADINSTR="$LHADINSTR
echo "LEPINSTR="$LEPINSTR

ROOTSYS=/home/joosep/root
#PATH=/home/joosep/local/include:/home/joosep/local/bin:/home/joosep/local/lib:$ROOTSYS/lib:$PATH
#LD_LIBRARY_PATH=/home/joosep/local/lib:$LD_LIBRARY_PATH
source ~/root/bin/thisroot.sh
cp -R /home/joosep/solarNu/pythia8170 $WD
cd $WD
cd genSun
LD_LIBRARY_PATH=/home/joosep/local/lib:/home/joosep/local/lib64:$LD_LIBRARY_PATH ./genSun.exe $PARTID $DMMASS output.root cardSunBatch.card $HHADINSTR $LHADINSTR $LEPINSTR &> log \
&& cp output.root /home/joosep/solarNu/out/output_$SLURM_JOB_ID.root
mv log /home/joosep/solarNu/out/log_$SLURM_JOB_ID.txt

ls -al
rm -Rf $WD
