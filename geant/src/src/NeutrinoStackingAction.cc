#include "globals.hh"
#include "G4Track.hh"

#include "NeutrinoStackingAction.hh"
#include "NeutrinoHistogram.hh"

#include "G4Electron.hh"
#include "G4Gamma.hh"

extern bool p_quiet;

NeutrinoStackingAction::NeutrinoStackingAction(bool verbose) : verbose(verbose) {
	if(!p_quiet) {
		G4cout << "Track killing on (" << (verbose ? "verbose" : "quiet") << ")" << G4endl;
	}
}

G4ClassificationOfNewTrack NeutrinoStackingAction::ClassifyNewTrack(const G4Track* tr) {
	if(!p_quiet && verbose) {
		G4cout << " T> "
		       << tr->GetParentID() << " -> " << tr->GetTrackID()
		       << " (" << tr->GetParticleDefinition()->GetParticleName() << ")"
		       << " @ E=" << tr->GetKineticEnergy()/MeV << "[MeV]"
		       << G4endl;
	}
	
	if(tr->GetParticleDefinition() == G4Electron::Electron() && tr->GetKineticEnergy() < 100*MeV) {
		if(!p_quiet&&verbose){G4cout << " T> " << tr->GetTrackID() << " -- KILL THIS ABOMINATION!!!!" << G4endl;}
		return fKill;
	} else if(tr->GetParticleDefinition() == G4Gamma::Gamma() && tr->GetKineticEnergy() < 100*MeV) {
		if(!p_quiet&&verbose){G4cout << " T> " << tr->GetTrackID() << " -- KILL THIS ABOMINATION!!!!" << G4endl;}
		return fKill;
	}
	
	return fUrgent;
}
