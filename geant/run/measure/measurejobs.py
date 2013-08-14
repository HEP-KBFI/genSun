#!/usr/bin/env python
import os,itertools

chs = [
	1, 2, 3, 4, 5, 6,
	11, 13, 15,
	21, 22,
	23, 24,
	25
]
mss = [
	[10, 100, 500, 1000, 5000, 10000, 50000, 100000],
	[5, 6, 8, 15, 20, 25, 30, 40],
	[50, 60, 70, 80, 90, 110, 120, 130],
	[140, 150, 160, 180, 200, 220, 240, 260],
	[280, 300, 330, 360, 400, 450, 550, 600],
	[650, 700, 750, 800, 900, 1100, 1200, 1300],
	[1500, 1700, 2000, 2500, 3000, 4000, 6000, 7000],
	[8000, 9000, 12000, 15000, 20000, 30000]
]

print 'Channels:', len(chs)
print 'Massgroups:', len(mss)

cmds = ['sbatch ./measure.py {0} {1} -o{2}'
	.format(str(ch), ' '.join(map(str,ms)), 'measure_{0}_{1}-{2}.pickle'.format(ch,ms[0],ms[-1]))
	for ch,ms in itertools.product(chs,mss)]
print 'Jobs:', len(cmds)

map(os.system, cmds)
#for c in cmds: print c
