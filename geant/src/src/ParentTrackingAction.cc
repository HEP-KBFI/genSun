#include "ParentTrackingAction.hh"

#include <G4TrackingManager.hh>

extern bool p_quiet, p_trv;

ParentTrackInformation::ParentTrackInformation(G4int parentID) : parentID(parentID) {}

void ParentTrackingAction::PostUserTrackingAction(const G4Track * tr) {
	G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
	size_t nSeco = secondaries ? secondaries->size() : 0;
	
	if(!p_quiet && p_trv) {
		G4cout << "PostUserTrack: "
		       << tr->GetTrackID()
		       << " (" << tr->GetParticleDefinition()->GetParticleName() << ")"
		       << " @ E=" << tr->GetKineticEnergy()/MeV << "[MeV]"
		       << G4endl;
		G4cout << " + Nseco: " << nSeco << G4endl;
	}
	
	for(size_t i=0; i < nSeco; i++) {
		if(!p_quiet && p_trv) {
			G4cout << " + "
				   << (*secondaries)[i]->GetParticleDefinition()->GetParticleName()
				   << " (" << (*secondaries)[i]->GetParticleDefinition()->GetPDGEncoding() << ")"
				   << " @ E=" << (*secondaries)[i]->GetKineticEnergy()/MeV << "[MeV]"
				   << "from: " << (*secondaries)[i]->GetParentID()
				   << G4endl;
		}
		ParentTrackInformation* infoNew = new ParentTrackInformation(tr->GetParticleDefinition()->GetPDGEncoding());
		(*secondaries)[i]->SetUserInformation(infoNew);
	}
}
