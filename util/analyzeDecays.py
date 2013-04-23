#!/usr/bin/env python

#Opens the debug output (event table) from genSun and creates a corresponding
#table of neutrinos
import ROOT
import tables

if __name__=="__main__":
    filename = "pythia8175/genSun/output.root"
    fi = ROOT.TFile(filename)
    
    tree = fi.
        Get("mass_1000").
        Get("particle_24").
        Get("energyLoss_hhad_2_lhad_1_chlep_2").
        Get("events")
    
    if not tree:
        raise FileError("Could not find the event TTree in the file %f: maybe you didn't enable debug output?" % filename)
    
    class Particle(tables.IsDescription):
        energy = tables.Float32Col()
        selfId = tables.Int32Col()
        parentId = tables.Int32Col()

    #THe output file
    ofile = tables.openFile("neutrinos.hdf5", mode = "w", title = "Neutrinos")

    table = ofile.createTable("/", "neutrinos", Particle)
    particle = table.row

    #Create at most so many neutrino rows
    N_max = 10**5
    N_ev = 0

    #Loop over events
    for ev in tree:
    
        #Get the size of the neutrino energy vector
        n_nu = ev.particles_E.size()
        energies = ev.particles_E
        ids = ev.particles_id
        parents = ev.particles_parent
        
        #Loop over neutrinos in event
        for i in range(n_nu):
        
            #Assume if and parent vector are the same length
            E = energies[i]
            Id = ids[i]
            parent_Id = parents[i]
            
            particle['energy'] = E
            particle['selfId'] = Id
            particle['parentId'] = parent_Id
            particle.append()
        
        if N_ev>N_max:
            break
        if N_ev%10000==0:
            print N_ev
            ofile.flush()
        N_ev = N_ev + 1
    ofile.close()
