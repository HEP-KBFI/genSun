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
parser.add_argument('-t', '--job-time', type=float, default=30, help='run time of a job in minutes')
args = parser.parse_args()
print 'Arguments:', args

def factors(n):    
	return sorted(set(reduce(list.__add__, ([i, int(n)//i] for i in range(1, int(int(n)**0.5) + 1) if int(n) % i == 0))))

optruns = factors(args.events)
print 'Optimal runs:', optruns

def findOptRuns(T):
	fs = filter(lambda t: t>T, optruns)
	return fs[0] if len(fs)>0 else optruns[-1]

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
		demi = args.job_time*60/mean # number of events in half an hour
		optDemi = findOptRuns(demi)
		realTime = mean*optDemi/60
		jobs = int(args.events/optDemi)
		print '{0:>2d} {1:>6d} {2:>10.4f} {3:>12.2f} \033[1m{4:>6d}\033[0m {5:>5.2f} \033[1m{6:>6d}\033[0m'.format(ch, e, mean, demi, optDemi, realTime, jobs)
		
		totJobs += jobs
		if not e in totJobs_E:
			totJobs_E[e] = 0
		totJobs_E[e] += jobs
		
		d['calc'] = {
			'mean_time': mean, 'runs_per_30min': demi,
			'rounded_runs_per_30min': optDemi, 'rounded_time': realTime,
			'jobs': jobs
		}

print 'Job count distribution:', totJobs_E
print 'Total jobs (N, N/5k, N/clust.hours, N/clust.days):', totJobs, totJobs/5e3, (args.job_time/60)*totJobs/5e3, (args.job_time/(60*24))*totJobs/5e3
print 'Target job time: {0} min'.format(args.job_time)

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
	
	chTotJobs = 0
	for m,d in sorted(es.items()):
		print >>fhtml, '<tr>'
		print >>fhtml, '<td>{0}</td>'.format(ch)
		print >>fhtml, '<td>{0}</td>'.format(m)
		for k,v in d['calc'].items():
			print >>fhtml, '<td>{0}</td>'.format(v)
		print >>fhtml, '</tr>'
		
		chTotJobs += d['calc']['jobs']
	
	sumstring = 'Nodes needed for 100k: <b>{0}</b>'.format(chTotJobs)
	print >>fhtml, '<tr><td colspan="{0}"><i>{1}</i></td></tr>'.format(len(keys), sumstring)
	
	print >>fhtml, '</table>'
print >>fhtml, '</body>'
print >>fhtml, '</html>'
fhtml.close()

fscript = open('submitscript.sh', 'w')
fscript.write('#!/bin/bash\n\n')
print >>fscript, '# Target events:', str(int(args.events/1e3))+'k'
print >>fscript, '# Target job time: {0} min'.format(args.job_time)
print >>fscript, '# Total jobs (N, N/5k, N/clust.days):', totJobs, totJobs/5e3, (args.job_time/(60*24))*totJobs/5e3
print >>fscript, '# Job count distribution:', totJobs_E

for (ch,es) in o.items():
	for E,d in sorted(es.items()):
		c = d['calc']
		print >>fscript, './sendjob -n{2} -r{3} {0} {1}'.format(ch, E, c['jobs'], c['rounded_runs_per_30min'])
fscript.close()
