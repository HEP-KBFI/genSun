#ifndef NeutrinoStackingAction_h
#define NeutrinoStackingAction_h

#include "G4UserStackingAction.hh"

class NeutrinoStackingAction : public G4UserStackingAction {
	public:
		G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
};

#endif
