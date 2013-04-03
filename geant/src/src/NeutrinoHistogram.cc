#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

#include "NeutrinoHistogram.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

NeutrinoHistogram::NeutrinoHistogram() {
	//
}

NeutrinoHistogram::~NeutrinoHistogram() {}

void NeutrinoHistogram::addParticle(const G4Track* tr) {
	nu type = NeutrinoHistogram::getNeutrinoType(tr->GetParticleDefinition());
	
	if(type != nuNot) {
		G4String pname = tr->GetParticleDefinition()->GetParticleName();
		G4double energy = tr->GetTotalEnergy();
		G4cout << " >>> Found: " << energy << " (" << pname << ")" << G4endl;
	}
}

nu NeutrinoHistogram::getNeutrinoType(const G4ParticleDefinition* g4def) {
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
