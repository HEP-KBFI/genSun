#!/usr/bin/env python
import os,subprocess,argparse

parser = argparse.ArgumentParser()
parser.add_argument('pid', type=int)
parser.add_argument('mass', type=str)
parser.add_argument('-n', '--jobs', type=int, default=1)
parser.add_argument('-r', '--runs', type=int, default=1)
parser.add_argument('-p', '--partition', type=str, default='main')
parser.add_argument('--dry', action='store_const', const=True, default=False)
args = parser.parse_args()
print 'Arguments:', args

if not os.path.isdir('jobs'): os.mkdir('jobs')
if not os.path.isfile('jobs/pythia.card'): os.symlink('../../src/scripts/pythia.card', 'jobs/pythia.card')
os.chdir('jobs')

# Build the sbatch command:
cmd  = ['sbatch']
cmd += ['-p{0}'.format(args.partition)]
cmd += ['../newbatch.sh']
cmd += ['-n{0}'.format(args.runs)]
cmd += [str(args.pid), args.mass]
print 'CMD list:', cmd
print 'Command:', ' '.join(cmd)

print 'Will submit {0} jobs.'.format(args.jobs)
submitted = 0
while submitted < args.jobs:
	print 'Jobs submitted: {0} out of {1} ({2:.0f}%)'.format(submitted, args.jobs, 100*float(submitted)/float(args.jobs))
	if args.dry:
		print 'Dry run:', ' '.join(cmd)
		submitted += 1
	else:
		if subprocess.call(cmd) == 0: submitted += 1
		else: print 'Error submitting. Not increasing counter..'
print 'All {0} jobs submitted done!'.format(args.jobs)
