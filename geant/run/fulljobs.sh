#!/bin/bash

# Gammas
#sbatch -n100   batch.sh -r100000 22 1      # --
sbatch -n100   batch.sh -r10000  22 10     # ~  2 sec / run (from: 0:15 / 10 runs)
sbatch -n100   batch.sh -r1000   22 100    # ~ 12 sec / run (from: 1:50 / 10 runs)
#sbatch -n100   batch.sh -r1000   22 1000   # --
#sbatch -n2000  batch.sh -r50     22 10000  # --

# Z bosons
#sbatch -n200   batch.sh -r10000  23 1000   # --

# Quarks - top
#sbatch -n100   batch.sh -r10000  6 1000    # --
