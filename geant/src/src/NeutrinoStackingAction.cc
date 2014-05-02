#include "globals.hh"
#include "G4Track.hh"

#include "NeutrinoStackingAction.hh"
#include "NeutrinoHistogram.hh"

#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4VProcess.hh"

extern bool p_quiet;

NeutrinoStackingAction::NeutrinoStackingAction(bool verbose) : verbose(verbose) {
	if(!p_quiet) {
		G4cout << "Track killing on (" << (verbose ? "verbose" : "quiet") << ")" << G4endl;
	}
}

G4ClassificationOfNewTrack NeutrinoStackingAction::ClassifyNewTrack(const G4Track* tr) {
	if(!p_quiet && verbose) {
		G4cout << "Track: "
		       << tr->GetParentID() << " -> " << tr->GetTrackID()
		       << " (" << tr->GetParticleDefinition()->GetParticleName() << ")"
		       << " @ E=" << tr->GetKineticEnergy()/MeV << "[MeV]"
		       << G4endl;
		if(tr->GetCreatorProcess() == NULL) {
			G4cout << " + No creator!" << G4endl;
		} else {
			G4cout << " + ProcessName      : " << tr->GetCreatorProcess()->GetProcessName() << G4endl;
			G4cout << " + ProcessType      : " << G4VProcess::GetProcessTypeName(tr->GetCreatorProcess()->GetProcessType()) << G4endl;
			G4cout << " + GetProcessSubType: " << tr->GetCreatorProcess()->GetProcessSubType() << G4endl;
			
		}
	}
	
	if(tr->GetParticleDefinition() == G4Electron::Electron() && tr->GetKineticEnergy() < 100*MeV) {
		if(!p_quiet&&verbose){G4cout << " + " << tr->GetTrackID() << " -- KILL THIS ABOMINATION!!!!" << G4endl;}
		return fKill;
	} else if(tr->GetParticleDefinition() == G4Gamma::Gamma() && tr->GetKineticEnergy() < 100*MeV) {
		if(!p_quiet&&verbose){G4cout << " + " << tr->GetTrackID() << " -- KILL THIS ABOMINATION!!!!" << G4endl;}
		return fKill;
	}
	
	return fUrgent;
}
