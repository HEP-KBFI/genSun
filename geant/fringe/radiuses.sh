#!/bin/bash
# Testing the effect of the radius
function run {
	# $1 - pid, $2 - radius, $3 - N
	echo "./solnugeant ${1} 10e3 -n${3} -q --radius=${2} -ohist_r${2}_p${1}_e10e3.root > hist_r${2}_p${1}_e10e3.log &"
	./solnugeant ${1} 10e3 -n${3} -q --radius=${2} -ohist_r${2}_p${1}_e10e3.root > hist_r${2}_p${1}_e10e3.log &
}

run 6  1e3 300
run 6  1e7 300
run 12 1e3 1000000
run 12 1e7 1000000
run 15 1e3 300
run 15 1e7 300
