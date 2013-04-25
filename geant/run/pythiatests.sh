#!/bin/bash

# Z boson
sbatch -n100 batch.sh -pVAC -r100000 23 1   # 10M events
sbatch -n100 batch.sh -pVAC -r100000 23 1000   # 10M events
sbatch -n100 batch.sh -pVAC -r100000 23 100000   # 10M events

# W boson
sbatch -n100 batch.sh -pVAC -r100000 24 1000 # 10M events
sbatch -n100 batch.sh -pVAC -r100000 24 100000 # 10M events

# H boson
sbatch -n100 batch.sh -pVAC -r50000 25 100000   # 5M events

# top quark
sbatch -n100 batch.sh -pVAC -r10000 6 100000   # 1M events
