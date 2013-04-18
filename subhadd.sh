DIR=`readlink -f $1`
parallel -n 100 sbatch parhadd.sh -- `find $DIR -name "*.root"`
