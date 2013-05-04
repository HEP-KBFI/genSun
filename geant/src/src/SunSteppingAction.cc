#include <numeric>

#include "globals.hh"
#include "G4Step.hh"
#include "G4Run.hh"

#include "SunSteppingAction.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

SunSteppingAction::SunSteppingAction(bool verbose) : verbose(verbose) {}

bool is_neutrino(const G4ParticleDefinition* g4def) {
	return ( g4def == G4NeutrinoE::NeutrinoEDefinition()
	       || g4def == G4AntiNeutrinoE::AntiNeutrinoEDefinition()
	       || g4def == G4NeutrinoMu::NeutrinoMuDefinition()
	       || g4def == G4AntiNeutrinoMu::AntiNeutrinoMuDefinition()
	       || g4def == G4NeutrinoTau::NeutrinoTauDefinition()
	       || g4def == G4AntiNeutrinoTau::AntiNeutrinoTauDefinition()
	);
}

void SunSteppingAction::UserSteppingAction(const G4Step* step) {
	G4double r0 = step->GetPreStepPoint()->GetPosition().mag();
	G4double r = step->GetPostStepPoint()->GetPosition().mag();
	G4String pname = step->GetTrack()->GetParticleDefinition()->GetParticleName();
	G4double E = step->GetTrack()->GetKineticEnergy();
	
	//G4cout << "Step at r0,r = (" << r0/m << ", " << r/m << ") m, P:" << pname << " (" << E/MeV << " MeV)" << G4endl;
	if(step->GetPostStepPoint()->GetStepStatus() == fWorldBoundary) {
		borderParticlesCounter++;
		if(is_neutrino(step->GetTrack()->GetParticleDefinition())) borderNeutrinosCounter++;
		
		if(verbose) G4cout << "World boundary step at r1>r2 = (" << r0/m << " > " << r/m << ") m, P:" << pname << " (" << E/MeV << " MeV)" << G4endl;
	}
}

void SunSteppingAction::BeginOfRunAction(const G4Run* run) {
	borderParticlesCounter = 0;
	borderNeutrinosCounter = 0;
}

void SunSteppingAction::EndOfRunAction(const G4Run* run) {
	borderParticles.push_back(borderParticlesCounter);
	borderNeutrinos.push_back(borderNeutrinosCounter);
}

void SunSteppingAction::statistics() {
	G4int borderParticlesTotal = std::accumulate(borderParticles.begin(),borderParticles.end(),0);
	G4int borderNeutrinosTotal = std::accumulate(borderNeutrinos.begin(),borderNeutrinos.end(),0);
	
	G4cout << "Total particles on world border: " << borderParticlesTotal << G4endl;
	G4cout << "Total neutrinos on world border: " << borderNeutrinosTotal << G4endl;
}
