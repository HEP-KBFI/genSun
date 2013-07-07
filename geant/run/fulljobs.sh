#!/bin/bash

Tn=$1 # unit: 30-60min, 100k events
Nn=1

echo "Tn=$Tn, Nn=$Nn"

function sendjob {
	# Arguments:
	#  - $1: pid
	#  - $2: E
	#  - $3: batches
	#  - $4: nodes / unit ($Nn)
	#  - $5: runs / unit ($Tn)
	for ((i=0;i<$3;i++)); do
		sbatch -n$(($4*$Nn)) batch.sh -r$(($5*$Tn)) $1 $2
	done
}

# Gammas
sendjob 22 10     1 1     100000 # ~ 0.02 sec / run (from 23 sec / 1000 runs)
sendjob 22 100    1 10    10000  # ~ 0.20 sec / run (from 20 sec / 100 runs)
sendjob 22 1000   1 100   1000   # ~ 2.00 sec / run (from 20 sec / 10 runs)

# Gluons
sendjob 21 10     1 5     20000  # ~ 0.08 sec / run (from 1:22 / 1000 runs)
sendjob 21 100    1 20    5000   # ~ 0.42 sec / run (from 42 sec / 100 runs)
sendjob 21 1000   1 100   1000   # ~ 2.60 sec / run (from 26 sec / 10 runs)

# Gauge bosons (Z = 23, W = 24)
sendjob 23 100    1 20    5000   # ~ 0.31 sec / run (from 31 sec / 100 runs)
sendjob 23 1000   1 100   1000   # ~ 1.51 sec / run (from 2:31 / 100 runs)
sendjob 24 100    1 20    5000   # ~ 0.27 sec / run (from 27 sec / 100 runs)
sendjob 24 1000   1 100   1000   # ~ 1.58 sec / run (from 2:38 / 100 runs)

# Higgs boson
sendjob 25 1000   1 100   1000   # ~ 1.78 sec / run (from 2:58 / 100 runs)

# Quarks (d,u,s,c,b)
for p in 1 2 3 4 5; do
	sendjob $p 10     1 4     25000  # ~ 0.08 sec / run (from 1:17 / 1000 runs with b)
	sendjob $p 100    1 20    5000   # ~ 0.37 sec / run (from 37 sec / 100 runs with b)
	sendjob $p 1000   1 100   1000   # ~ 2.04 sec / run (from 3:24 / 100 runs with b)
done

# t-quark
sendjob  6 1000   1 100   1000   # ~ 2.01 sec / run (from 3:21 / 100 runs)

# Leptons (11 = electron, 13 = muon, 15 = tau)
sendjob 11 10     1 1     100000 # ~ 0.02 sec / run (from 48 sec / 2000 runs)
sendjob 11 100    1 10    10000  # ~ 0.18 sec / run (from 3:04 / 1000 runs)
sendjob 11 1000   1 100   1000   # ~ 1.77 sec / run (from 2:57 / 100 runs)
sendjob 11 10000  2 500   100    # ~ 17.9 sec / run (from 2:59 / 10 runs)
#sendjob 11 100000 20 500   10     # ~ 171 sec / run (from 2:51 / 1 run) [??]

sendjob 13 10     1 1     100000 # ~ 0.01 sec / run (from 21 sec / 2500 runs)
sendjob 13 100    1 4     25000  # ~ 0.07 sec / run (from 68 sec / 1000 runs)
sendjob 13 1000   1 50    2000   # ~ 0.94 sec / run (from 1:34 / 100 runs)
sendjob 13 10000  2 400   125    # ~ 14.3 sec / run (from 2:23 / 10 runs)
#sendjob 13 100000 20 500  10     # ~ 171 sec / run (from 2:51 / 1 run)

sendjob 15 10     1 2     50000  # ~ 0.03 sec / run (from 32 sec / 1000 runs) [??]
sendjob 15 100    1 5     20000  # ~ 0.14 sec / run (from 14 sec / 100 runs)
sendjob 15 1000   1 50    2000   # ~ 0.95 sec / run (from 1:35 / 100 runs)
sendjob 15 10000  1 500   200    # ~ 9.75 sec / run (from 3:15 / 20 runs)
#sendjob 15 100000 5 500   40     # ~ 51   sec / run (from 51 sec / 1 run)
