#ifndef ParentTrackAction_h
#define ParentTrackAction_h

#include <globals.hh>
#include <G4Track.hh>
#include <G4VUserTrackInformation.hh>
#include <G4UserTrackingAction.hh>

struct ParentTrackInformation : public G4VUserTrackInformation {
	G4int parentID;
	ParentTrackInformation(G4int parentID);
};

class ParentTrackingAction : public G4UserTrackingAction {
	void PostUserTrackingAction(const G4Track * tr);
};

#endif
