#ifndef G4ParentTrackInformationAction_h
#define G4ParentTrackInformationAction_h

#include "G4UserTrackingAction.hh"
#include "G4VUserTrackInformation.hh"

struct ParentTrackInformation : public G4VUserTrackInformation {
	G4int parentTrackID, parentPDGID;
	ParentTrackInformation(G4int parentTrackID, G4int parentPDGID);
};

class G4ParentTrackInformationAction : public G4UserTrackingAction {
	public:
		void PostUserTrackingAction(const G4Track* aTrack);
};

#endif
