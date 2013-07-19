#!/usr/bin/env python
import pickle, math
fin = open('merged.pkl')
o = pickle.load(fin)
fin.close()

import argparse_local as argparse
parser = argparse.ArgumentParser()
parser.add_argument('-l', '--low', type=float, default=0)
parser.add_argument('-u', '--high', type=float, default=1e5)
parser.add_argument('-e', '--events', type=float, default=1e5)
parser.add_argument('-N', '--max-nodes', type=float, default=500)
args = parser.parse_args()
print 'Arguments:', args

def factors(n):    
	return sorted(set(reduce(list.__add__, ([i, int(n)//i] for i in range(1, int(int(n)**0.5) + 1) if int(n) % i == 0))))

optruns = factors(args.events)
print 'Optimal runs:', optruns

def findOptRuns(T):
	fs = filter(lambda t: t>T, optruns)
	return fs[0] if len(fs)>0 else optruns[-1]

def minimizeOpt(n):
	j = 1
	while n > args.max_nodes:
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
	return m > args.low and m <= args.high
	#return m in [5,10,50,100,500,1000,5000,10000,50000,100000]

for ch in o:
	o[ch] = dict(filter(filter_fn,  o[ch].items()))

totNodes = 0; totNodes_E = {}
totJobs = 0; totJobs_E = {}
for (ch,es) in o.items():
	es = o[ch]
	for (e,d) in sorted(es.items()):
		mean = sum(d['dts'])/(len(d['dts'])*d['N'])
		demi = 30*60/mean # number of events in half an hour
		optDemi = findOptRuns(demi)
		realTime = mean*optDemi/60
		parallel = int(args.events/optDemi)
		jobs,nodes = minimizeOpt(parallel)
		print '{0:>2d} {1:>6d} {2:>10.4f} {3:>12.2f} \033[1m{4:>6d}\033[0m {5:>5.2f} {6:>6d} \033[1m{7:3d}\033[0m \033[1m{8:3d}\033[0m'.format(ch, e, mean, demi, optDemi, realTime, parallel, jobs, nodes)
		
		totNodes += parallel
		if not e in totNodes_E:
			totNodes_E[e] = 0
		totNodes_E[e] += parallel
		
		totJobs += jobs
		if not e in totJobs_E:
			totJobs_E[e] = 0
		totJobs_E[e] += jobs
		
		d['calc'] = {
			'mean_time': mean, 'runs_per_30min': demi,
			'rounded_runs_per_30min': optDemi, 'rounded_time': realTime,
			'parallel_threads': parallel, 'jobs': jobs, 'nodes_per_job': nodes
		}

print 'Node count distribution:', totNodes_E
print 'Job count distribution:', totJobs_E

print 'Total nodes (N, N/5k, N/clust.days):', totNodes, totNodes/5e3, (totNodes/5e3)/24
print 'Total cluster jobs:', totJobs


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
	
	chTotNodes = 0
	for m,d in sorted(es.items()):
		print >>fhtml, '<tr>'
		print >>fhtml, '<td>{0}</td>'.format(ch)
		print >>fhtml, '<td>{0}</td>'.format(m)
		for k,v in d['calc'].items():
			print >>fhtml, '<td>{0}</td>'.format(v)
		print >>fhtml, '</tr>'
		
		chTotNodes += d['calc']['parallel_threads']
	
	sumstring = 'Nodes needed for 100k: <b>{0}</b>'.format(chTotNodes)
	print >>fhtml, '<tr><td colspan="{0}"><i>{1}</i></td></tr>'.format(len(keys), sumstring)
	
	print >>fhtml, '</table>'
print >>fhtml, '</body>'
print >>fhtml, '</html>'
fhtml.close()

fscript = open('submitscript.sh', 'w')
fscript.write('#!/bin/bash\n\n')
print >>fscript, '# Target events:', str(int(args.events/1e3))+'k'
print >>fscript, '# Total nodes (N, N/5k, N/clust.days):', totNodes, totNodes/5e3, (totNodes/5e3)/24
print >>fscript, '# Total cluster jobs:', totJobs
print >>fscript, '# Node count distribution:', totNodes_E
print >>fscript, '# Job count distribution:', totJobs_E
fscript.write('''
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
