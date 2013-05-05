#include <stdexcept>
#include <map>

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TH1F.h"

#include "DMRootHistogrammer.hh"

extern bool p_quiet;

struct pinfo {
	const char * name;
	TH1F * h;
	
	pinfo(G4int id, const char * name) : name(name) {
		h = NULL;
	}
};

std::map<G4int, pinfo> particles = {
	/*{12, pinfo(12,   "nu_e")}, {-12, pinfo(-12,   "anti_nu_e")},
	{14, pinfo(14,  "nu_mu")}, {-14, pinfo(-14,  "anti_nu_mu")},
	{16, pinfo(16, "nu_tau")}, {-16, pinfo(-16, "anti_nu_tau")}*/
	{12, pinfo(12,   "nue")}, {-12, pinfo(-12,   "anue")},
	{14, pinfo(14,  "numu")}, {-14, pinfo(-14,  "anumu")},
	{16, pinfo(16, "nutau")}, {-16, pinfo(-16, "anutau")}
};

DMRootHistogrammer::DMRootHistogrammer(G4int channel_id, G4double dm_mass, const char * physics, HistParams energyhist, HistParams statushist)
: channel(channel_id), dm_mass(dm_mass), physics(physics) {
	char hname[50];
	
	// create histograms for all particles
	for(auto it = particles.begin(); it != particles.end(); ++it) {
		sprintf(hname, "%s", it->second.name);
		it->second.h = new TH1F(
			hname, energyhist.title,
			energyhist.nbins, energyhist.xmin, energyhist.xmax
		);
		if(!p_quiet){G4cout << "Histogram `" << hname << "` for pid=" << it->first << G4endl;}
		hists.push_back(it->second.h);
	}
	
	// create the event status histogram
	sprintf(hname, "eventStatus");
	h_evstatus = new TH1F(
		hname, statushist.title,
		statushist.nbins, statushist.xmin, statushist.xmax
	);
	if(!p_quiet){G4cout << "Histogram `" << hname << "` for ev.status" << G4endl;}
	hists.push_back(h_evstatus);
}

DMRootHistogrammer::~DMRootHistogrammer() {}

void DMRootHistogrammer::addParticle(const G4Track* tr) {
	G4double energy = tr->GetTotalEnergy();
	G4double logE = log10(energy/dm_mass);
	const G4ParticleDefinition* pdef = tr->GetParticleDefinition();
	G4int pdgid = pdef->GetPDGEncoding();
	G4String pname = pdef->GetParticleName();
	
	if(!p_quiet){G4cout << " > Adding particle: " << energy/MeV << "[MeV] (" << pdgid << ", " << pname << ") -- logE = " << logE << G4endl;}
	
	try {
		particles.at(pdgid).h->Fill(logE);
	} catch(std::out_of_range &e) {
		if(!p_quiet){G4cout << " > Bad particle - ho histogram!" << G4endl;}
	}
}

void DMRootHistogrammer::countEvent(const G4int n) {
	if(!p_quiet){G4cout << "Add event (" << n << ")" << G4endl;}
	h_evstatus->Fill(0.0, (double)n);
}

void DMRootHistogrammer::save(const char* name) {
	char dirname_mass[50], dirname_particle[50], dirname_physics[50];
	sprintf(dirname_mass, "mass_%i%s",  (dm_mass<GeV) ? (int)(dm_mass/MeV) : (int)(dm_mass/GeV), (dm_mass<GeV)?"M":"");
	sprintf(dirname_particle, "particle_%i", channel);
	sprintf(dirname_physics, "physics_%s", physics);
	
	if(!p_quiet){
		G4cout << "TDirectory(mass): " << dirname_mass << G4endl;
		G4cout << "TDirectory(particle): " << dirname_particle << G4endl;
		G4cout << "TDirectory(physics): " << dirname_physics << G4endl;
	}
	
	TFile tfile(name, "RECREATE");
	TDirectory* dir = tfile.mkdir(dirname_mass)->mkdir(dirname_particle)->mkdir(dirname_physics);
	dir->cd();
	
	for(auto it = hists.begin(); it != hists.end(); ++it) {
		(*it)->Write();
	}
	
	tfile.Close();
}
