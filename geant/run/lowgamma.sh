#!/bin/bash

# Low energy gammas
sbatch -n100 batch.sh -r1000000 22 1 -uM     # ~ 2 sec / 1000 runs
sbatch -n100 batch.sh -r1000000 22 10 -uM    # ~ 2 sec / 1000 runs
sbatch -n100 batch.sh -r100000  22 100 -uM   # ~10 sec / 1000 runs
sbatch -n100 batch.sh -r10000   22 1000 -uM  # ~10 sec /  100 runs

