#!/bin/bash
uname -a
WD=/scratch/joosep/solarNuWD-$SLURM_JOB_ID
source /cvmfs/cms.cern.ch/slc5_amd64_gcc472/lcg/root/5.34.04/bin/thisroot.sh
source /cvmfs/cms.cern.ch/cmsset_default.sh

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

ROOTSYS=/home/joosep/root
#PATH=/home/joosep/local/include:/home/joosep/local/bin:/home/joosep/local/lib:$ROOTSYS/lib:$PATH
#LD_LIBRARY_PATH=/home/joosep/local/lib:$LD_LIBRARY_PATH
source ~/root/bin/thisroot.sh
cp -R /home/joosep/solarNu/pythia8170 $WD
cd $WD
cd genSun
LD_LIBRARY_PATH=/home/joosep/local/lib:/home/joosep/local/lib64:$LD_LIBRARY_PATH ./genSun.exe $PARTID $DMMASS output.root cardSunBatch.card $HHADINSTR $LHADINSTR $LEPINSTR &> log \
&& cp output.root $OFDIR/output_$SLURM_JOB_ID.root
mv log $OFDIR/log_$SLURM_JOB_ID.txt

ls -al
rm -Rf $WD
