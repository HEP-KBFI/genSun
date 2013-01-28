OF=out
mkdir $OF
particles=( 1 5 6 11 12 13 15 22 23 24 25  )
for i in "${particles[@]}"
do
	LD_LIBRARY_PATH=/home/joosep/local/lib:$LD_LIBRARY_PATH time ./genSun.exe $i 1000 $OF/output$i.root gammaPlusX.card &> $OF/log$i & 
done


