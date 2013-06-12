#!/bin/bash
#echo "Args:" $@
var=$(time (./solnugeant -q $@ > /dev/null) 2>&1 )
echo "T" $var "for" $@
