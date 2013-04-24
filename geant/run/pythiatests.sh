#!/bin/bash

# Z bosons
sbatch -n100 batch.sh -pTRANS -r100000 23 1   # 10M events
#sbatch -n100 batch.sh -pTRANS -r100000 23 10   # 10M events
#sbatch -n100 batch.sh -pTRANS -r100000 23 100   # 10M events
sbatch -n100 batch.sh -pTRANS -r100000 23 1000   # 10M events
#sbatch -n100 batch.sh -pTRANS -r100000 23 10000   # 10M events
sbatch -n100 batch.sh -pTRANS -r100000 23 100000   # 10M events
