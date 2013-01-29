DMMASS=1000

OF="out_m_"$DMMASS
mkdir $OF
particles=( 1 5 6 11 12 13 22 23 24 25  )
for i in "${particles[@]}"
do
	time ./genSun.exe $i $DMMASS $OF/output$i.root gammaPlusX.card 2 1 2 &> $OF/log$i & 
done


