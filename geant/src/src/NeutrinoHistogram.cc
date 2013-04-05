#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4RootAnalysisManager.hh"

#include "NeutrinoHistogram.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

extern bool p_quiet;

struct neutrino_info {
	nu_t type;
	G4String name;
};
neutrino_info neutrinos[] = {
	{nuE, "nu_e"},     {nuEBar, "anti_nu_e"},
	{nuMu, "nu_mu"},   {nuMuBar, "anti_nu_mu"},
	{nuTau, "nu_tau"}, {nuTauBar, "anti_nu_tau"}
};


NeutrinoHistogram::NeutrinoHistogram(
	G4String ofile,
	G4int pid, G4double dm_mass,
	G4double xmin, G4double xmax,
	G4int nbins,
	bool logstyle
) {
	char hname[100];
	G4int nucount = sizeof(neutrinos)/sizeof(neutrinos[0]);
	
	anm = new G4RootAnalysisManager();
	anm->OpenFile(ofile);
	if(!p_quiet){G4cout << "Opening root file: " << ofile << G4endl;}
	
	hs = new G4int[nucount];
	for(int i=0; i < nucount; i++) {
		neutrino_info nu = neutrinos[i];
		sprintf(hname, "energy_%s_p%i_m%i", nu.name.data(), abs(pid), (int)(dm_mass/GeV));
		if(!p_quiet){G4cout << " > H: " << hname << G4endl;}
		hs[nu.type] = anm->CreateH1(
			hname, "Neutrino energy spectrum", // name, title
			nbins, xmin/MeV, xmax/MeV, // nbins, xmin, xmax
			"MeV", "none" //unitName="none", fcnName="none"
		);
	}
}

NeutrinoHistogram::~NeutrinoHistogram() {
	anm->Write();
	anm->CloseFile();
	delete[] hs;
}

void NeutrinoHistogram::addParticle(const G4Track* tr) {
	nu_t type = NeutrinoHistogram::getNeutrinoType(tr->GetParticleDefinition());
	
	if(type != nuNot) {
		G4String pname = tr->GetParticleDefinition()->GetParticleName();
		G4double energy = tr->GetTotalEnergy();
		
		if(!p_quiet){G4cout << " > Adding neutrino: " << energy << " (" << pname << ")" << G4endl;}
		
		anm->FillH1(hs[type], energy); // id, value, weight=1.0
	}
}

nu_t NeutrinoHistogram::getNeutrinoType(const G4ParticleDefinition* g4def) {
	if( g4def == G4NeutrinoE::NeutrinoEDefinition() ) {
		return nuE;
	} else if( g4def == G4AntiNeutrinoE::AntiNeutrinoEDefinition() ) {
		return nuEBar;
	} else if( g4def == G4NeutrinoMu::NeutrinoMuDefinition() ) {
		return nuMu;
	} else if( g4def == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition() ) {
		return nuMuBar;
	} else if( g4def == G4NeutrinoTau::NeutrinoTauDefinition() ) {
		return nuTau;
	} else if( g4def == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition() ) {
		return nuTauBar;
	}
	
	return nuNot;
}
