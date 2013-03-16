#!/bin/bash
uname -a
WD=/scratch/joosep/solarNuWD-$SLURM_JOB_ID
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd ~/CMSSW_5_3_8_patch3
eval `scramv1 runtime -sh`
source /cvmfs/cms.cern.ch/slc5_amd64_gcc462/lcg/root/5.32.00-cms21/bin/thisroot.sh
export PYTHIA8DATA=/home/joosep/solarNu/pythia8175/xmldoc/
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
#LD_LIBRARY_PATH=/home/joosep/local/lib:/home/joosep/local/lib64:$LD_LIBRARY_PATH ./genSun.exe $PARTID $DMMASS output.root cardSunBatch.card $HHADINSTR $LHADINSTR $LEPINSTR &> log \
 ./genSun.exe $PARTID $DMMASS output.root cardSunBatch.card $HHADINSTR $LHADINSTR $LEPINSTR &> log \
&& cp output.root $OFDIR/output_$SLURM_JOB_ID.root
mv log $OFDIR/log_$SLURM_JOB_ID.txt

ls -al
rm -Rf $WD
