#!/bin/bash

# Low energy gammas
sbatch -n100 -uM  batch.sh -r1000000 22 1      # ~ 2 sec / 1000 runs
sbatch -n100 -uM  batch.sh -r1000000 22 10     # ~ 2 sec / 1000 runs
sbatch -n100 -uM  batch.sh -r100000  22 100    # ~10 sec / 1000 runs
sbatch -n100 -uM  batch.sh -r10000   22 1000   # ~10 sec /  100 runs

