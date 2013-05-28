DIR=`readlink -f $1`
#parallel -n 100 sbatch /home/joosep/solarNu/util/parhadd.sh -- `find $DIR -name "*.root"`
find $DIR -name "out*.root" -print0 | xargs -n50 -0 sbatch /home/joosep/solarNu/util/parhadd.sh
