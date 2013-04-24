#!/bin/bash

# Gammas
sbatch -n100   batch.sh -r100000 22 1      # 20min/10000 runs
sbatch -n100   batch.sh -r10000  22 10
sbatch -n50    batch.sh -r2000   22 100    # ~10s per run; 100k events
sbatch -n100   batch.sh -r1000   22 1000   # ~1.5m per run; 100k events
sbatch -n2000  batch.sh -r50     22 10000  # ~15m per run; 100k events

# Z bosons
sbatch -n200   batch.sh -r10000  23 1000   # check; 2M events
