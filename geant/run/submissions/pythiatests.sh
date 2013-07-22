#!/bin/bash

# Low energy electons(11), muons(13) and gammas(22)
sbatch -n10 batch.sh -pVAC -r1000000 11 10    # ~ 8s/1000 runs
sbatch -n10 batch.sh -pVAC -r1000000 11 100   # ~ 8s/1000 runs
sbatch -n10 batch.sh -pVAC -r1000000 11 1000  # ~ 8s/1000 runs

sbatch -n10 batch.sh -pVAC -r1000000 13 10    # ~ 8s/1000 runs
sbatch -n10 batch.sh -pVAC -r1000000 13 100   # ~ 8s/1000 runs
sbatch -n10 batch.sh -pVAC -r1000000 13 1000  # ~ 8s/1000 runs

sbatch -n10 batch.sh -pVAC -r1000000 22 10    # ~ 6s/1000 runs
sbatch -n10 batch.sh -pVAC -r1000000 22 100   # ~ 6s/1000 runs
sbatch -n10 batch.sh -pVAC -r1000000 22 1000  # ~ 6s/1000 runs

# Z boson
sbatch -n100 batch.sh -pVAC -r100000 23 1000   # 10M events
sbatch -n100 batch.sh -pVAC -r100000 23 100000   # 10M events

# W boson
sbatch -n100 batch.sh -pVAC -r100000 24 1000 # 10M events
sbatch -n100 batch.sh -pVAC -r100000 24 100000 # 10M events

# H boson
sbatch -n100 batch.sh -pVAC -r50000 25 100000   # 5M events

# top quark
sbatch -n100 batch.sh -pVAC -r10000 6 100000   # 1M events
