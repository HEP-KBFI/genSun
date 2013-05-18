#!/bin/bash

# Low energy gammas
#sbatch -n100 batch.sh -r1000000 22 1   -uM -cGEANT4    # ~ 1 sec / 1000 runs (from: ~10s / 10 000 runs)
sbatch -n100 batch.sh -r1000000 22 10  -uM -cGEANT4    # ~ 2 sec / 1000 runs (from: ~15s / 10 000 runs)
sbatch -n100 batch.sh -r1000000 22 100 -uM -cGEANT4    # ~10 sec / 1000 runs
sbatch -n100 batch.sh -r100000  22 200 -uM -cGEANT4    # ~25 sec / 1000 runs
sbatch -n100 batch.sh -r100000  22 500 -uM -cGEANT4    # ~ 6 sec /  100 runs
#sbatch -n100 batch.sh -r100000  22 800 -uM -cGEANT4    # ~10 sec /  100 runs
#sbatch -n250 batch.sh -r40000   22 1000 -uM -cGEANT4   # ~10 sec /  100 runs

# Low energy electrons
sbatch -n100 batch.sh -r1000000 11 1    -uM -cGEANT4   # ~ 1 sec / 1000 runs (from: ~10s / 10 000 runs)
sbatch -n100 batch.sh -r1000000 11 10   -uM -cGEANT4   # ~ 2 sec / 1000 runs (from: ~16s / 10 000 runs)
sbatch -n100 batch.sh -r100000  11 100  -uM -cGEANT4   # ~10 sec / 1000 runs
sbatch -n200 batch.sh -r50000   11 1000 -uM -cGEANT4   # ~12 sec /  100 runs

# Low energy protons
sbatch -n20 batch.sh -r5000   2212 1   -cGEANT4      # ~25 sec /  10 runs
sbatch -n40 batch.sh -r2500   2212 10  -cGEANT4      # ~ 7 sec /   1 run
