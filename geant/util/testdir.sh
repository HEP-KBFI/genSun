#!/bin/bash
#SBATCH -J "solnuG4" -x comp-c-[002,004,006,009-012,014,016,020,022,024,025,028,031,032,034,036,037],comp-d-[001,083,098,111,117]


echo $0
echo $PWD

SWD=$( dirname $PWD/$0 )
echo $SWD
$SWD/solnugeant

echo ${BASH_SOURCE[0]}
