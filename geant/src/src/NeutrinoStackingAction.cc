#include "globals.hh"
#include "G4Track.hh"

#include "NeutrinoStackingAction.hh"
#include "NeutrinoHistogram.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

NeutrinoStackingAction::NeutrinoStackingAction(NeutrinoHistogram* hist) {
	this->histogram = hist;
}

G4ClassificationOfNewTrack NeutrinoStackingAction::ClassifyNewTrack(const G4Track* aTrack) {
	nu type = NeutrinoHistogram::getNeutrinoType(aTrack->GetParticleDefinition());
	histogram->addParticle(aTrack);
	return fUrgent;
}
