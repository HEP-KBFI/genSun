#/usr/bin/bash

function sendparticle {
	# $1 / $2 / $3 - pid / E / -r
	sbatch ../batch.sh --store -r$3 $1 $2
}

for p in 5 11 13 15; do
	sendparticle $p 10    100000
	sendparticle $p 100   10000
	sendparticle $p 1000  5000
	sendparticle $p 10000 1000
done

sendparticle 6 1000  1000
sendparticle 6 10000 500

sendparticle 24 100   10000
sendparticle 24 1000  5000
sendparticle 24 10000 1000
