#!/bin/env python
import glob, re
from datetime import datetime

dfm = '%a %b %d %H:%M:%S %Z %Y'

slurmfiles = sorted(glob.glob('slurm-*'))

_ = lambda x: x
ms_p = [
	('runs',      'runs=(.*)',        int),
	('physics',   'physics=(.*)',     _  ),
	('particle',  'particle=(.*)',    _  ),
	('energy',    'energy=(.*)',      _  ),
	('prefix',    'prefix=(.*)',      _  ),
	('phystring', 'PHYS_STR=\"(.*)\"',  _  )
]

for sf in slurmfiles:
	m_jobid = int(re.match('slurm-(.*).out', sf).group(1))
	
	fh = open(sf, 'r')
	lines = fh.readlines()
	
	# Filter and match:
	ms = []
	def mfn(p,f,s):
		m = re.search(p,s)
		return f(m.group(1)) if m is not None else None
	for ln in lines[:200]:
		rd=map(lambda(k,p,f): (k,mfn(p,f,ln)), ms_p)
		ms += filter(lambda (k,s): s is not None, rd)
		if len(set(map(lambda(k,v):k, ms)))==len(ms_p): break
	ms=dict(ms)
	
	m_runs = ms['runs']
	m_physics = ms['physics']
	m_particle = ms['particle']
	m_energy = ms['energy']
	m_prefix = ms['prefix']
	m_phystring = ms['phystring'] if 'phystring' in ms else '---'
	
	date_start = datetime.strptime(lines[1].strip(), dfm)
	try:
		date_end = datetime.strptime(lines[-1].strip(), dfm)
		delta = date_end - date_start
		run_time = (date_end - date_start)/m_runs
	except ValueError:
		date_end = 'NOTFINISHED'
		delta = '+'+str(datetime.now().replace(microsecond=0) - date_start)
		run_time = 'NOTFINISHED'
	
	print 'J {0}: {1:>4} {2:>7} {3:>5} {4:>10}  {5:>14}  {6}  {7}'.format(m_jobid, m_particle, m_energy, m_physics, delta, run_time, date_start, m_phystring)
	#print 'Start/end: %s / %s'%(date_start, date_end)
	#print 'Delta: %s (%s /run)'%(delta, run_time)
	
	fh.close()
