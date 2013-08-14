#!/bin/bash
for x in {0..30}; do
	#for y in 1 2; do
	#	./solnugeant -n10000 13 10 > log_${x}_${y} &
	#done
	./solnugeant -n10000 13 10 > log_${x} &
	sleep 2
done
