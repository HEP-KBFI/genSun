#include "G4TrackingManager.hh"
#include "G4ParentTrackInformationAction.hh"

extern bool p_quiet;

void G4ParentTrackInformationAction::PostUserTrackingAction(const G4Track* aTrack) {
	if(!p_quiet){G4cout << "PostUserTrackingAction(trackID=" << aTrack->GetTrackID() << ") called!" << G4endl;}
	
	G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
	if(secondaries) {
		size_t nSeco = secondaries->size();
		for(size_t i=0; i < nSeco; i++) {
			if(!p_quiet) {
				G4cout << " >> sec: " << (*secondaries)[i]->GetTrackID()
				       << " from " << (*secondaries)[i]->GetParentID()
				       << " is " << (*secondaries)[i]->GetParticleDefinition()->GetParticleName()
				       << " (" << (*secondaries)[i]->GetParticleDefinition()->GetPDGEncoding() << ")"
				       << G4endl;
			}
			
			ParentTrackInformation * parent_info = new ParentTrackInformation(
				aTrack->GetTrackID(),
				aTrack->GetParticleDefinition()->GetPDGEncoding()
			);
			(*secondaries)[i]->SetUserInformation(parent_info);
		}
	}
}

ParentTrackInformation::ParentTrackInformation(G4int parentTrackID, G4int parentPDGID)
: parentTrackID(parentTrackID), parentPDGID(parentPDGID) {
	if(!p_quiet){G4cout << "new ParentTrackInformation(" << parentTrackID << ", " << parentPDGID << ")" << G4endl;}
}
