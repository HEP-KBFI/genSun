#!/bin/bash

# Low energy gammas
#sbatch -n100 batch.sh -r1000000 22 1   -uM -cGEANT4 --trk=off   # ~ 0.90  sec / 1000 runs (from 45 sec / 50k runs)
sbatch -n10  batch.sh -r1000000 22 10  -uM -cGEANT4 --trk=off   # ~ 1.46 sec / 1000 runs (from 73 sec / 50k runs)
sbatch -n10  batch.sh -r1000000 22 50  -uM -cGEANT4 --trk=off   # ~ 0.01 sec / run (from 28 sec / 5000 runs)
sbatch -n10  batch.sh -r1000000 22 100 -uM -cGEANT4 --trk=off   # ~ 0.01 sec / run (from 50 sec / 5000 runs)
sbatch -n20  batch.sh -r500000  22 150 -uM -cGEANT4 --trk=off   # ~ 0.02 sec / run (from 38 sec / 2500 runs)
sbatch -n100 batch.sh -r100000  22 200 -uM -cGEANT4 --trk=off   # ~ 0.03 sec / run (from 66 sec / 2500 runs)
sbatch -n100 batch.sh -r100000  22 500 -uM -cGEANT4 --trk=off   # ~ 0.06 sec / run (from 56 sec / 1000 runs)
#sbatch -n100 batch.sh -r100000  22 800 -uM -cGEANT4 --trk=off   # ~10 sec /  100 runs
sbatch -n100 batch.sh -r100000   22 1000 -uM -cGEANT4 --trk=off  # ~ 0.13 sec / run (from 128 sec / 1000 runs)

# Low energy electrons
#sbatch -n100 batch.sh -r1000000 11 1    -uM -cGEANT4 --trk=off  # ~ 0.88 sec / 1000 runs (from 88 sec / 100k runs)
sbatch -n10  batch.sh -r1000000 11 10   -uM -cGEANT4 --trk=off  # ~ 1.42 sec / 1000 runs (from 71 sec / 50k runs)
sbatch -n10  batch.sh -r1000000 11 50   -uM -cGEANT4 --trk=off  # ~ 5.30 sec / 1000 runs (from 53 sec / 10k runs)
sbatch -n10  batch.sh -r1000000 11 100  -uM -cGEANT4 --trk=off  # ~ 9.70 sec / 1000 runs (from 97 sec / 10k runs)
sbatch -n20  batch.sh -r500000  11 150  -uM -cGEANT4 --trk=off  # ~ 0.01 sec / run (from 71 sec / 5000 runs)
sbatch -n100 batch.sh -r100000  11 200  -uM -cGEANT4 --trk=off  # ~ 0.02 sec / run (from 93 sec / 5000 runs)
sbatch -n100 batch.sh -r100000  11 500  -uM -cGEANT4 --trk=off  # ~ 0.05 sec / run (from 50 sec / 1000 runs)
sbatch -n100 batch.sh -r100000  11 1000 -uM -cGEANT4 --trk=off  # ~ 0.09 sec / run (from 98 sec / 1000 runs)

# Low energy protons
sbatch -n200 batch.sh -r5000   2212 1   -cGEANT4 --trk=off     # ~ 2.2 sec / run (from 43 sec / 20 runs)
#sbatch -n20 batch.sh -r5000   2212 2   -cGEANT4 --trk=off     # ~?? sec /  10 runs
#sbatch -n40 batch.sh -r2500   2212 10  -cGEANT4 --trk=off     # ~ 7 sec /   1 run
