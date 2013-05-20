while true
do
	vq=`squeue -umorten`
	vr=`./report.py`

	clear
	date; echo
	echo "$vq"; echo
	echo "$vr"

	sleep 10
done
