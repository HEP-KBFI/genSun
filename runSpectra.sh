DMMASS=1000

OF="out_m_"$DMMASS
mkdir $OF
particles=( 1 5 6 11 12 13 15 22 23 24 25  )
for i in "${particles[@]}"
do
	#Hadrons probabilistic, light hadrons stop, ch. leptons probabilistic
	time ./genSun.exe $i $DMMASS $OF/output_prob$i.root gammaPlusX.card 2 1 2 &> $OF/log$i & 
	
	#heavy hadrons average, light hadrons stop, ch. leptons average 
        time ./genSun.exe $i $DMMASS $OF/output_avg$i.root gammaPlusX.card 1 1 1 &> $OF/log$i & 

	#no energy loss
	time ./genSun.exe $i $DMMASS $OF/output_no$i.root gammaPlusX.card 0 0 0 &> $OF/log$i & 
done


