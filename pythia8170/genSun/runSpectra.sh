DMMASS=5000

OF="out_m_"$DMMASS
mkdir $OF
#particles=( 1 5 6 11 12 13 15 22 23 24 25  )
particles=( 6 15 )
for i in "${particles[@]}"
do
	#Hadrons probabilistic, light hadrons stop, ch. leptons probabilistic
	./genSun.exe $i $DMMASS $OF/output_prob$i.root cardSunBatch.card 2 1 2 &> $OF/log_prob$i & 
	
	#heavy hadrons average, light hadrons stop, ch. leptons average 
    ./genSun.exe $i $DMMASS $OF/output_avg$i.root cardSunBatch.card 1 1 1 &> $OF/log_avg$i & 

	#no energy loss
	./genSun.exe $i $DMMASS $OF/output_no$i.root cardSunBatch.card 0 0 0 &> $OF/log_no$i & 
done


