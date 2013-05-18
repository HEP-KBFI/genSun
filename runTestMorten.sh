
OFDIR=/home/joosep/solarNu/out_morten_test
if [ -d $OFDIR ]; then
    rm -Rf $OFDIR
fi
mkdir $OFDIR
WDIR=pythia8176/genSun/
runSpec() {

    $WDIR/genSun.exe $1 $2 $OFDIR/output_prob_$i_$2.root $WDIR/cardSunBatch.card 0 0 0 $3 &> $OFDIR/log_prob_$1_$2.txt &
}

runSpec 11 10 10000000
runSpec 11 1000 5000000
runSpec 13 100 5000000
runSpec 22 10 1000000
runSpec 22 1000 100000
runSpec 24 1000 1000000
runSpec 6 100000 100000
