#ifndef NeutrinoStackingAction_h
#define NeutrinoStackingAction_h

#include "G4UserStackingAction.hh"

class NeutrinoHistogram;

class NeutrinoStackingAction : public G4UserStackingAction {
	NeutrinoHistogram* histogram;
	
	public:
		NeutrinoStackingAction(NeutrinoHistogram* hist);
		G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
};

#endif
