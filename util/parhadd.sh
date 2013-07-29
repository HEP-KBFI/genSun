#!/bin/bash
#cd /home/joosep/CMSSW_5_3_8_patch3
#eval `scramv1 runtime -sh`
#source /cvmfs/cms.cern.ch/slc5_amd64_gcc462/lcg/root/5.32.00-cms21/bin/thisroot.sh
source /home/joosep/solarNu/setenv.sh
cd $SLURM_SUBMIT_DIR
OUTNAME="out_"$SLURM_JOB_ID".root"
hadd -k $OUTNAME $@
echo "done hadding"
