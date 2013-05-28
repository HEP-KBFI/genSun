#!/bin/bash

# Gammas
#sbatch -n100   batch.sh -r100000 22 1      # --
#sbatch -n100   batch.sh -r100000  22 10     # ~ 0.02 sec / run (from 23 sec / 1000 runs)
#sbatch -n100   batch.sh -r10000   22 100    # ~ 0.2  sec / run (from 20 sec / 100 runs)
#sbatch -n100   batch.sh -r1000   22 1000   # --
#sbatch -n2000  batch.sh -r50     22 10000  # --

# Z bosons
#sbatch -n200   batch.sh -r10000  23 1000   # --

# Quarks - top
#sbatch -n100   batch.sh -r10000  6 1000     # ~ 2    sec / run (from 200 sec / 100 runs)

# Leptons (13 = muon, 15 = tau)
sbatch -n40    batch.sh -r250000 13 10       # ~ 0.01 sec / run (from 21 sec / 2500 runs)
sbatch -n100   batch.sh -r100000 13 100      # ~ 0.07 sec / run (from 68 sec / 1000 runs)
sbatch -n100   batch.sh -r10000  13 1000     # ~ 0.94 sec / run (from 1:34 / 100 runs)
sbatch -n200   batch.sh -r1000   13 10000    #
#sbatch -n200   batch.sh -r50     13 100000   #

sbatch -n40    batch.sh -r250000 15 10       # ~ 0.03 sec / run (from 32 sec / 1000 runs)
sbatch -n200   batch.sh -r50000  15 100      # ~ 0.14 sec / run (from 14 sec / 100 runs)
sbatch -n100   batch.sh -r10000  15 1000     # ~ 0.95 sec / run (from 1:35 / 100 runs)
sbatch -n200   batch.sh -r1000   15 10000    # ~ 9.75 sec / run (from 3:15 / 20 runs)
#sbatch -n400   batch.sh -r250    15 100000   # ~60    sec / run (from 60 sec / 1 run)
