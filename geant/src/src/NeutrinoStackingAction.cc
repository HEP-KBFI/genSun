#include "globals.hh"
#include "G4Track.hh"

#include "NeutrinoStackingAction.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

G4ClassificationOfNewTrack NeutrinoStackingAction::ClassifyNewTrack(const G4Track* aTrack) {
	G4String pname = aTrack->GetParticleDefinition()->GetParticleName();
	G4double energy = aTrack->GetTotalEnergy();
	
	if( aTrack->GetParticleDefinition() == G4NeutrinoE::NeutrinoEDefinition()
	 || aTrack->GetParticleDefinition() == G4AntiNeutrinoE::AntiNeutrinoEDefinition()
	 || aTrack->GetParticleDefinition() == G4NeutrinoMu::NeutrinoMuDefinition()
	 || aTrack->GetParticleDefinition() == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()
	 || aTrack->GetParticleDefinition() == G4NeutrinoTau::NeutrinoTauDefinition()
	 || aTrack->GetParticleDefinition() == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()
	) {
		G4cout << " >>> Found: " << energy << " (" << pname << ")" << G4endl;
		return fKill;
	}
	return fUrgent;
}
