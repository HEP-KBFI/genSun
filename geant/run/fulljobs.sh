#!/bin/bash

# Gammas
sbatch -n100   batch.sh -r100000  22 10     # ~ 0.02 sec / run (from 23 sec / 1000 runs)
sbatch -n100   batch.sh -r10000   22 100    # ~ 0.20 sec / run (from 20 sec / 100 runs)
sbatch -n100   batch.sh -r1000    22 1000   # ~ 2.00 sec / run (from 20 sec / 10 runs)

# Gluons
sbatch -n100   batch.sh -r10000   21 10     # ~ 0.08 sec / run (from 1:22 / 1000 runs)
sbatch -n100   batch.sh -r10000   21 100    # ~ 0.42 sec / run (from 42 sec / 100 runs)
sbatch -n100   batch.sh -r1000    21 1000   # ~ 2.60 sec / run (from 26 sec / 10 runs)

# Gauge bosons (Z = 23, W = 24)
sbatch -n100   batch.sh -r10000   23 100    # ~ 0.31 sec / run (from 31 sec / 100 runs)
sbatch -n100   batch.sh -r1000    23 1000   # ~ 1.51 sec / run (from 2:31 / 100 runs)
sbatch -n100   batch.sh -r10000   24 100    # ~ 0.27 sec / run (from 27 sec / 100 runs)
sbatch -n100   batch.sh -r1000    24 1000   # ~ 1.58 sec / run (from 2:38 / 100 runs)

# Higgs boson
sbatch -n100   batch.sh -r1000    25 1000   # ~ 1.78 sec / run (from 2:58 / 100 runs)

# Quarks (d,u,s,c,b)
for p in 1 2 3 4 5; do
	sbatch -n100   batch.sh -r10000  $p 10      # ~ 0.08 sec / run (from 1:17 / 1000 runs with b)
	sbatch -n100   batch.sh -r10000  $p 100     # ~ 0.37 sec / run (from 37 sec / 100 runs with b)
	sbatch -n100   batch.sh -r10000  $p 1000    # ~ 2.04 sec / run (from 3:24 / 100 runs with b)
done

# t-quark
sbatch -n100   batch.sh -r10000   6 1000    # ~ 2.01 sec / run (from 3:21 / 100 runs)

# Leptons (11 = electron, 13 = muon, 15 = tau)
sbatch -n100   batch.sh -r100000 11 10       # ~ 0.02 sec / run (from 48 sec / 2000 runs)
sbatch -n100   batch.sh -r10000  11 100      # ~ 0.18 sec / run (from 3:04 / 1000 runs)
sbatch -n100   batch.sh -r1000   11 1000     # ~ 1.77 sec / run (from 2:57 / 100 runs)
sbatch -n500   batch.sh -r200    11 10000    # ~ 17.9 sec / run (from 2:59 / 10 runs)
#sbatch -n200   batch.sh -r50     11 100000   # ~ 171 sec / run (from 2:51 / 1 run) [??]

#sbatch -n40    batch.sh -r250000 13 10       # ~ 0.01 sec / run (from 21 sec / 2500 runs) [??]
sbatch -n100   batch.sh -r10000  13 100      # ~ 0.07 sec / run (from 68 sec / 1000 runs)
sbatch -n100   batch.sh -r1000   13 1000     # ~ 0.94 sec / run (from 1:34 / 100 runs)
sbatch -n200   batch.sh -r500    13 10000    # ~ 14.3 sec / run (from 2:23 / 10 runs)
#sbatch -n200   batch.sh -r50     13 100000   # ~ 171 sec / run (from 2:51 / 1 run)

#sbatch -n40    batch.sh -r250000 15 10       # ~ 0.03 sec / run (from 32 sec / 1000 runs) [??]
sbatch -n200   batch.sh -r50000  15 100      # ~ 0.14 sec / run (from 14 sec / 100 runs)
sbatch -n100   batch.sh -r10000  15 1000     # ~ 0.95 sec / run (from 1:35 / 100 runs)
sbatch -n200   batch.sh -r1000   15 10000    # ~ 9.75 sec / run (from 3:15 / 20 runs)
#sbatch -n400   batch.sh -r250    15 100000   # ~ 51   sec / run (from 51 sec / 1 run)
