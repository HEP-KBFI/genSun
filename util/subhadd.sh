DIR=`readlink -f $1`
#parallel -n 100 sbatch /home/joosep/solarNu/util/parhadd.sh -- `find $DIR -name "*.root"`
find $DIR -name "out*.root" -print0 | xargs -n400 -0 sbatch -p phys /home/joosep/solarNu/util/parhadd.sh
