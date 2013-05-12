#!/bin/env python
import glob, re
from datetime import datetime

dfm = '%a %b %d %H:%M:%S %Z %Y'

slurmfiles = sorted(glob.glob('slurm-*'))

for sf in slurmfiles:
	#print '==========================   %s   =========================='%(sf)
	fh = open(sf, 'r')
	lines = fh.readlines()
	lines = map(lambda x: x.strip(), lines)
	
	m_jobid = int(re.match('slurm-(.*).out', sf).group(1))
	m_runs = int(re.match('runs=(.*)', lines[4]).group(1))
	m_physics = re.match('physics=(.*)', lines[5]).group(1)
	m_particle = re.match('particle=(.*)', lines[6]).group(1)
	m_energy = re.match('energy=(.*)', lines[7]).group(1)
	m_prefix = re.match('prefix=(.*)', lines[8]).group(1)
	
	date_start = datetime.strptime(lines[1], dfm)
	try:
		date_end = datetime.strptime(lines[-1], dfm)
		delta = date_end - date_start
		run_time = (date_end - date_start)/m_runs
	except ValueError:
		date_end = 'NOTFINISHED'
		delta = '+'+str(datetime.now().replace(microsecond=0) - date_start)
		run_time = 'NOTFINISHED'
	
	print 'J {0}: {1:>2} {2:>7} {3:>5} {4:>10}  {5:>14}  {6}'.format(m_jobid, m_particle, m_energy, m_physics, delta, run_time, date_start)
	#print 'Start/end: %s / %s'%(date_start, date_end)
	#print 'Delta: %s (%s /run)'%(delta, run_time)
	
	fh.close()
