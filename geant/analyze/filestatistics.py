#!/usr/bin/env python
import rootpy, rootpy.io
import os, re, math
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('fs', type=str, nargs='+')
args = parser.parse_args()

pcs = {}

print 'List of files:'
for f in args.fs:
	print '   ',f
print

for f in args.fs:
	print 'Stats on file `%s`'%f
	tfile = rootpy.io.root_open(f)
	pattern = re.compile('^mass_([0-9A-Za-z]*)/particle_([0-9]*)/physics_([A-Za-z0-9_]*)$')
	for x in tfile.walk():
		match = pattern.match(x[0])
		if not match:
			continue
		
		mass = int(match.group(1))
		pid  = int(match.group(2))
		phys = match.group(3)
		
		d = tfile.Get(x[0])
		evs = d.Get('eventStatus').GetBinContent(1)
		#print x[0],evs, math.log10(evs)
		print '{0:>6} {1:>2} {2:>10} {3:>5.2f}'.format(mass, pid, phys, math.log10(evs))
		
		key = (mass,pid)
		if key in pcs:
			pcs[key] += evs
		else:
			pcs[key] = evs
	tfile.Close()
	print

def print_summary_table(pcs, log=True):
	print 'Summary table:'
	masses = sorted(set([k[0] for k in pcs.keys()]))
	pids   = sorted(set([k[1] for k in pcs.keys()]))

	print '  mass',
	for pid in pids:
		if log:
			print '    {0:>2}'.format(pid),
		else:
			print '          {0:>2}'.format(pid),
	print

	for m in masses:
		print '{0:>6}'.format(m),
		for pid in pids:
			key = (m,pid)
			if key in pcs:
				if log:
					print '{0:>6.2f}'.format(math.log10(pcs[key])),
				else:
					print '{0:>12d}'.format(int(pcs[key])),
			else:
				if log:
					print '     .',
				else:
					print '           .',
		print
	print

print_summary_table(pcs)
print_summary_table(pcs, False)
