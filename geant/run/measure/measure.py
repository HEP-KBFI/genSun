#!/usr/bin/env python
#SBATCH -J "sng4-msr" -x comp-c-[002,004,006,009-012,014,016,020,022,024,025,028,031,032,034,036,037],comp-d-[001,083,098,111,117]
import os,time,random,pickle

import argparse_local as argparse
parser = argparse.ArgumentParser()
parser.add_argument('channel', type=int)
parser.add_argument('mass', nargs='+', type=int)
parser.add_argument('--tmin', type=float, default=60)
parser.add_argument('-o', type=str, default='/dev/null')
args = parser.parse_args()
print args.channel, args.mass

minruntime = args.tmin
ch = args.channel
ms = args.mass

def measure(ch, E, n):
	seed = random.randint(0,1e8)
	cmd = './../../install/solnugeant -q -n{2} {0} {1} --seed={3} -o/dev/null &> /dev/null'.format(ch, E, n, seed)
	print 'Command:', cmd
	
	t0=time.time(); os.system(cmd); dt=time.time()-t0
	
	print dt, dt/n
	return dt

fout = open(args.o, 'w')

for m in ms:
	print 'Channel {0}, mass {1}'.format(ch,m)
	N = None
	
	out = {
		'channel': ch,'mass': m,
		'search': {}, 'N': None, 'dts': []
	}
	
	print 'Searching for the optimal N (tmin={0}) ...'.format(minruntime)
	for n in [2**i for i in range(0,30)]:
		dt = measure(ch, m, n)
		out['search'][n] = dt
		if dt > minruntime:
			N = n
			break
	
	if not N:
		print 'Error: could not determine N'
		continue
	
	out['N'] = N
	print 'Measure a few times...'
	for i in range(0,5):
		dt = measure(ch, m, N)
		out['dts'].append(dt)
	
	pickle.dump(out, fout)

fout.close()
