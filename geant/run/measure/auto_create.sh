#!/bin/bash
./analyze.py -l000 -u1e3 -e1e5; cp submitscript.sh ../auto_LOW.sh
./analyze.py -l1e3 -u1e4 -e1e5; cp submitscript.sh ../auto_MED.sh
./analyze.py -l1e4 -uinf -e1e5; cp submitscript.sh ../auto_HIGH.sh
./analyze.py -l1e4 -uinf -e1e4; cp submitscript.sh ../auto_HIGH_lowstat.sh
