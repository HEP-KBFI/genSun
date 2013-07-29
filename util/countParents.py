#!/usr/bin/env python
#Finds the neutrino parent frequency distribution in a certain energy range
import tables
from collections import Counter
import numpy

#Needs the pregenerated table
fi = tables.openFile("neutrinos.hdf5")

table = fi.root.neutrinos

print table
#Energy range (GeV)
E_low = 0.0003
E_high = 0.001
ids = numpy.array([x["parentId"] for x in
    table.where("(energy>%.2f) & (energy<%.2f)" % (E_low, E_high))]
)

x_bins = 5000*numpy.logspace(-9, 0, 100, base=10)
particles = set([])
print x_bins
parents = []
for i in range(len(x_bins)-1):
    s = "(energy > %f) & (energy <= %f)" % (x_bins[i], x_bins[i+1])
    p = [x["parentId"] for x in table.where(s)]
    c = Counter(p)
    parents.append(c)

for p in parents:
    for k in p.keys():
        if k not in particles:
            particles.add(k)

particles = list(particles)

for p in parents:
    for part in particles:
        if part not in p.keys():
            p[part]=0

s = "{"
for p in parents:
    s += "\n{"
    for part in particles:
        s += str(p[part]) + ", "
    s = s[:-2]
    s += "},"
s = s[:-1]
s += "\n}"

print particles
print s

print len([x["selfId"] for x in table.where("parentId==-211")])
print len([x["selfId"] for x in table.where("parentId==211")])