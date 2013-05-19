#!/bin/bash

# Gammas
#sbatch -n100   batch.sh -r100000 22 1      # --
sbatch -n100   batch.sh -r100000  22 10     # ~ 0.02 sec / run (from: 23 sec / 1000 runs)
sbatch -n100   batch.sh -r10000   22 100    # ~ 0.2  sec / run (from: 20 sec / 100 runs)
#sbatch -n100   batch.sh -r1000   22 1000   # --
#sbatch -n2000  batch.sh -r50     22 10000  # --

# Z bosons
#sbatch -n200   batch.sh -r10000  23 1000   # --

# Quarks - top
sbatch -n100   batch.sh -r10000  6 1000     # ~ 2    sec / run (from: 200 sec / 100 runs)
