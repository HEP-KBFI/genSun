#!/usr/bin/env python
import pickle, math
fin = open('merged.pkl')
o = pickle.load(fin)
fin.close()

optruns = [
	1, 2, 4, 5, 8, 10, 16, 20, 25, 32, 40, 50, 80, 100,
	125, 160, 200, 250, 400, 500, 625, 800, 1000, 1250,
	2000, 2500, 3125, 4000, 5000, 6250, 10000, 12500,
	20000, 25000, 50000, 100000
]

def findOptRuns(T):
	fs = filter(lambda t: t>T, optruns)
	return fs[0] if len(fs)>0 else optruns[-1]

def minimizeOpt(n):
	j = 1
	while n > 500:
		if n%5 == 0:
			n /= 5; j *= 5
		elif n%2 == 0:
			n /= 2; j *= 2
		else:
			raise Exception('Can not divide!')
	return (j,n)

# filter bad masses
o[6]  = dict(filter(lambda (k,v): k >= 175.0,  o[6].items()))
o[23] = dict(filter(lambda (k,v): k >=  92.0, o[23].items()))
o[24] = dict(filter(lambda (k,v): k >=  81.0, o[24].items()))
o[25] = dict(filter(lambda (k,v): k >= 127.0, o[25].items()))

def filter_fn((m,o)):
	#return m <= 5000
	return True

for ch in o:
	o[ch] = dict(filter(filter_fn,  o[ch].items()))

totNodes = 0
for (ch,es) in o.items():
	es = o[ch]
	for (e,d) in sorted(es.items()):
		mean = sum(d['dts'])/(len(d['dts'])*d['N'])
		demi = 30*60/mean # number of events in half an hour
		optDemi = findOptRuns(demi)
		realTime = mean*optDemi/60
		parallel = int(1e5/optDemi)
		jobs,nodes = minimizeOpt(parallel)
		print '{0:>2d} {1:>6d} {2:>10.4f} {3:>12.2f} \033[1m{4:>6d}\033[0m {5:>5.2f} {6:>6d} \033[1m{7:3d}\033[0m \033[1m{8:3d}\033[0m'.format(ch, e, mean, demi, optDemi, realTime, parallel, jobs, nodes)
		
		totNodes += parallel
		d['calc'] = {
			'mean_time': mean, 'runs_per_30min': demi,
			'rounded_runs_per_30min': optDemi, 'rounded_time': realTime,
			'parallel_threads': parallel, 'jobs': jobs, 'nodes_per_job': nodes
		}

print totNodes, totNodes/5e3, (totNodes/5e3)/24

fhtml = open('report.html', 'w')
print >>fhtml, '<html>'
print >>fhtml, '<head><title>SolNuGeant measure report</title></head>'
print >>fhtml, '<body>'
keys = ['channel', 'mass'] + o.values()[0].values()[0]['calc'].keys()
for (ch,es) in o.items():
	print >>fhtml, '<table border="1">'
	print >>fhtml, '<tr><th colspan="{0}">Channel: {1}</th></tr>'.format(len(keys), ch)
	
	print >>fhtml, '<tr>'
	for key in keys:
		print >>fhtml, '<th>{0}</th>'.format(key)
	print >>fhtml, '</tr>'
	
	totNodes = 0
	for m,d in sorted(es.items()):
		print >>fhtml, '<tr>'
		print >>fhtml, '<td>{0}</td>'.format(ch)
		print >>fhtml, '<td>{0}</td>'.format(m)
		for k,v in d['calc'].items():
			print >>fhtml, '<td>{0}</td>'.format(v)
		print >>fhtml, '</tr>'
		
		totNodes += d['calc']['parallel_threads']
	
	sumstring = 'Nodes needed for 100k: <b>{0}</b>'.format(totNodes)
	print >>fhtml, '<tr><td colspan="{0}"><i>{1}</i></td></tr>'.format(len(keys), sumstring)
	
	print >>fhtml, '</table>'
print >>fhtml, '</body>'
print >>fhtml, '</html>'
fhtml.close()

fscript = open('submitscript.sh', 'w')
fscript.write('''#!/bin/bash

Tn=$1 # unit: 30-60min, 100k events
Nn=1

echo "Tn=$Tn, Nn=$Nn"

function sendjob {
	# Arguments:
	#  - $1: pid
	#  - $2: E
	#  - $3: batches
	#  - $4: nodes / unit ($Nn)
	#  - $5: runs / unit ($Tn)
	for ((i=0;i<$3;i++)); do
		sbatch -n$(($4*$Nn)) batch.sh -r$(($5*$Tn)) $1 $2
	done
}\n\n''')

for (ch,es) in o.items():
	for E,d in sorted(es.items()):
		c = d['calc']
		print >>fscript, 'sendjob {0} {1} {2} {3} {4}'.format(ch, E, c['jobs'], c['nodes_per_job'], c['rounded_runs_per_30min'])
fscript.close()
