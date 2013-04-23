import ROOT
import tables
fi = ROOT.TFile("pythia8175/genSun/output.root")
tree = fi.Get("mass_1000").Get("particle_24").Get("energyLoss_hhad_2_lhad_1_chlep_2").Get("events")
class Particle(tables.IsDescription):
    energy = tables.Float32Col()
    selfId = tables.Int32Col()
    parentId = tables.Int32Col()
ofile = tables.openFile("neutrinos.hdf5", mode = "w", title = "Neutrinos")
table = ofile.createTable("/", "neutrinos", Particle)
particle = table.row
N_max = 10**4
N_ev = 0
for ev in tree:
    n_nu = ev.particles_E.size()
    energies = ev.particles_E
    ids = ev.particles_id
    parents = ev.particles_parent
    for i in range(n_nu):
        E = energies[i]
        if E<1.0:
            Id = ids[i]
            parent_Id = parents[i]
            particle['energy'] = E
            particle['selfId'] = Id
            particle['parentId'] = parent_Id
            particle.append()
            #s = "%.2f, %.2f, %.2f" % (E, Id, parent_Id)
            #ofile.write(s + "\n")
    if N_ev>N_max:
        break
    if N_ev%1000==0:
        print N_ev
        ofile.flush()
    N_ev = N_ev + 1
ofile.close()
#tree = fi.Get(
