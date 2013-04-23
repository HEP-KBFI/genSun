#!/usr/bin/env python
#Finds the neutrino parent frequency distribution in a certain energy range
import tables
from collections import Counter
import numpy

#Needs the pregenerated table
fi = tables.openFile("neutrinos.hdf5")

table = fi.root.neutrinos

#Energy range (GeV)
E_low = 0.0003
E_high = 0.001
ids = numpy.array([x["parentId"] for x in
    table.where("(energy>%.2f) & (energy<%.2f)" % (E_low, E_high))]
)

print Counter(ids)
