import tables
from collections import Counter
import numpy
fi = tables.openFile("neutrinos.hdf5")

table = fi.root.neutrinos

ids = numpy.array([x["parentId"] for x in table.where("(energy>0.0003) & (energy<0.001)")])
print Counter(ids)
