#!/usr/bin/env python
import os,time,random,pickle

import argparse_local as argparse
parser = argparse.ArgumentParser()
parser.add_argument('pickles', nargs='+', type=str)
args = parser.parse_args()

out = {}
for pFile in args.pickles:
	f = open(pFile)
	try:
		while True:
			o = pickle.load(f)
			if not o['channel'] in out:
				out[o['channel']] = {}
			out[int(o['channel'])][int(o['mass'])] = o
			
	except EOFError:
		pass
	f.close()

ofile = open('merged.pkl', 'w')
pickle.dump(out, ofile)
ofile.close()
