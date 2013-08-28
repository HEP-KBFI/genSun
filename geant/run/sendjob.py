#!/usr/bin/env python
import os
import os.path
import subprocess
import argparse
import slur

parser = argparse.ArgumentParser()
parser.add_argument('pid', type=int)
parser.add_argument('mass', type=str)
parser.add_argument('-n', '--jobs', type=int, default=1)
parser.add_argument('-r', '--runs', type=int, default=1)
parser.add_argument('-p', '--partition', type=str, default='main')
parser.add_argument('--dry', action='store_const', const=True, default=False)
parser.add_argument('--sbatch', action='store_const', const=True, default=False)
args = parser.parse_args()
print 'Arguments:', args

if not os.path.isdir('jobs'): os.mkdir('jobs')
if not os.path.isfile('jobs/pythia.card'): os.symlink('../../src/scripts/pythia.card', 'jobs/pythia.card')
os.chdir('jobs')

# Build the sbatch command:
sbatch = 'sbatch'
args_slurm = ['-p{0}'.format(args.partition)]
script = '../batch.sh'
args_script  = ['-r{0}'.format(args.runs)]
args_script += [str(args.pid), args.mass]

if args.sbatch:
	print 'Submitting directly!'
	cmd = [sbatch]+args_slurm+[script]+args_script
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
else:
	print 'Submitting to SLUR queue.'
	slurdb=slur.SlurDB()
	slurdb.queue_job(os.path.abspath(script), args_script, args_slurm, N=args.jobs)
print 'All {0} jobs submitted. Done!'.format(args.jobs)
