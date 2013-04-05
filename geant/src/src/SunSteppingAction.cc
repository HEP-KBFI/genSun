#include "globals.hh"
#include "G4Step.hh"

#include "SunSteppingAction.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

SunSteppingAction::SunSteppingAction(bool verbose) : verbose(verbose) {}

void SunSteppingAction::UserSteppingAction(const G4Step* step) {
	G4double r0 = step->GetPreStepPoint()->GetPosition().mag();
	G4double r = step->GetPostStepPoint()->GetPosition().mag();
	G4String pname = step->GetTrack()->GetParticleDefinition()->GetParticleName();
	G4double E = step->GetTrack()->GetKineticEnergy();
	
	//G4cout << "Step at r0,r = (" << r0/m << ", " << r/m << ") m, P:" << pname << " (" << E/MeV << " MeV)" << G4endl;
	if(step->GetPostStepPoint()->GetStepStatus() == fWorldBoundary) {
		if(verbose) G4cout << "World boundary step at r1>r2 = (" << r0/m << " > " << r/m << ") m, P:" << pname << " (" << E/MeV << " MeV)" << G4endl;
		
	}
}
