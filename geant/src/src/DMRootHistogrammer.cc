#include <stdexcept>
#include <map>
#include <vector>

#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TH1F.h"

#include "DMRootHistogrammer.hh"
#include "DMEventWriter.hh"
#include "G4ParentTrackInformationAction.hh"

extern bool p_quiet;

static ParentTrackInformation pythia_userinfo_pointer(0, 0);

struct pinfo {
	const char * name;
	TH1F * h;
	bool b_enh, b_cnth;
	double ch_val; int ch_bin;
	
	pinfo(G4int id, const char * name, bool energyhist=true, bool counthist=true)
	: name(name), b_enh(energyhist), b_cnth(counthist) {
		h = NULL;
	}
};

struct pdgid_compare {
	bool operator() (const G4int& lhs, const G4int& rhs) const {
		if(abs(lhs) == abs(rhs)) {
			return lhs > rhs;
		} else {
			return abs(lhs) < abs(rhs);
		}
	}
};

std::map<G4int, pinfo, pdgid_compare> particles = {
	{12, pinfo(12,  "nuel")}, {-12, pinfo(-12,  "anuel")},
	{14, pinfo(14,  "numu")}, {-14, pinfo(-14,  "anumu")},
	{16, pinfo(16, "nutau")}, {-16, pinfo(-16, "anutau")},
	{11, pinfo(11,   "ele", false)}, {-11, pinfo(-11,   "aele", false)},
	{13, pinfo(13,    "mu", false)}, {-13, pinfo(-13,    "amu", false)},
	{15, pinfo(15,   "tau", false)}, {-15, pinfo(-15,   "atau", false)},
	{22, pinfo(22, "gamma", false)},
	{2112, pinfo(2112, "neutron", false)}, {-2112, pinfo(-2112, "aneutron", false)},
	{2212, pinfo(2212,  "proton", false)}, {-2212, pinfo(-2212,  "aproton", false)},
};

DMRootHistogrammer::DMRootHistogrammer(G4int channel_id, G4double dm_mass, const char * physics, bool store_events, HistParams energyhist, HistParams statushist)
: channel(channel_id), dm_mass(dm_mass), physics(physics), store_events(store_events) {
	char hname[50], binname[50];
	
	if(store_events) {
		this->evWriter = new DMEventWriter();
	}
	
	// create histograms for all particles
	for(auto it = particles.begin(); it != particles.end(); ++it) {
		sprintf(hname, "%s", it->second.name);
		it->second.h = new TH1F(
			hname, energyhist.title,
			energyhist.nbins, energyhist.xmin, energyhist.xmax
		);
		if(!p_quiet){G4cout << "Histogram `" << hname << "` for pid=" << it->first << G4endl;}
		//hists.push_back(it->second.h);
	}
	
	// create particle counter histogram
	int n_lbl_bins = 1;
	for(auto it = particles.begin(); it != particles.end(); ++it) {
		if(it->second.b_cnth) {
			it->second.ch_val = n_lbl_bins + 0.5;
			it->second.ch_bin = n_lbl_bins + 1;
			n_lbl_bins++;
		} else {
			it->second.ch_val = 0.5;
		}
	}
	h_pcounter = new TH1F(
		"particlecounter", "Particles",
		n_lbl_bins, 0, n_lbl_bins
	);
	h_pcounter->GetXaxis()->SetBinLabel(1, "other");
	for(auto it = particles.begin(); it != particles.end(); ++it) {
		if(!it->second.b_cnth) continue;
		sprintf(binname, "%i (%s)", it->first, it->second.name);
		h_pcounter->GetXaxis()->SetBinLabel(it->second.ch_bin, binname);
	}
	//hists.push_back(h_pcounter);
	
	// create the event status histogram
	sprintf(hname, "eventStatus");
	h_evstatus = new TH1F(
		hname, statushist.title,
		statushist.nbins, statushist.xmin, statushist.xmax
	);
	if(!p_quiet){G4cout << "Histogram `" << hname << "` for ev.status" << G4endl;}
	//hists.push_back(h_evstatus);
}

DMRootHistogrammer::~DMRootHistogrammer() {}

void DMRootHistogrammer::addParticle(const G4Track* tr) {
	G4double energy = tr->GetTotalEnergy();
	G4double logE = log10(energy/dm_mass);
	const G4ParticleDefinition* pdef = tr->GetParticleDefinition();
	G4int pdgid = pdef->GetPDGEncoding();
	G4String pname = pdef->GetParticleName();
	
	if(!p_quiet){G4cout << " > Adding particle: " << energy/MeV << "[MeV] (" << pdgid << ", " << pname << ") -- logE = " << logE << G4endl;}
	
	if(store_events) {
		ParentTrackInformation * userinfo = (ParentTrackInformation*) tr->GetUserInformation();

		if(userinfo == NULL) {
			userinfo = &pythia_userinfo_pointer;
		}

		if(tr->GetCreatorProcess() == NULL) {
			if(!p_quiet){G4cout << "Process: PYTHIA8" << G4endl;}
		} else {
			if(!p_quiet){
				G4cout << tr->GetCreatorProcess()->GetProcessName()
				       << " - " << G4VProcess::GetProcessTypeName(tr->GetCreatorProcess()->GetProcessType())
				       << " (" << tr->GetCreatorProcess()->GetProcessType() << ")"
				       << ", " << tr->GetCreatorProcess()->GetProcessSubType()
				       << " - parent id/pdg: " << userinfo->parentTrackID << " / " << userinfo->parentPDGID
				       << G4endl;
			}
		}

		evWriter->fill(0, pdgid, energy, userinfo->parentPDGID);
	}
	
	try {
		pinfo p = particles.at(pdgid);
		p.h->Fill(logE);
		h_pcounter->Fill(p.ch_val);
	} catch(std::out_of_range &e) {
		h_pcounter->Fill(0.5);
		if(!p_quiet){G4cout << " > Bad particle - not listed!" << G4endl;}
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
	TDirectory* dir_o = dir->mkdir("others");
	dir->cd();
	
	/*for(auto it = hists.begin(); it != hists.end(); ++it) {
		(*it)->Write();
	}*/
	
	h_evstatus->Write();
	h_pcounter->Write();
	
	for(auto it = particles.begin(); it != particles.end(); ++it) {
		if(it->second.b_enh) {
			it->second.h->Write();
		} else {
			dir_o->WriteTObject(it->second.h);
		}
	}
	
	tfile.Close();
	
	if(store_events) {
		delete this->evWriter;
	}
}
