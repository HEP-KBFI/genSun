#ifndef NeutrinoStackingAction_h
#define NeutrinoStackingAction_h

#include "G4UserStackingAction.hh"

class NeutrinoHistogram;

class NeutrinoStackingAction : public G4UserStackingAction {
	bool verbose;
	
	public:
		NeutrinoStackingAction(bool verbose);
		G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* tr);
};

#endif
