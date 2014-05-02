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
#include "G4VProcess.hh"
#include "G4SteppingManager.hh"
#include "G4VUserTrackInformation.hh"

#include "DMRootHistogrammer.hh"

extern bool p_quiet, p_trv;

SunSteppingAction::SunSteppingAction(DMRootHistogrammer* hgr) : hgr(hgr) {}

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

	if(!p_quiet && p_trv) {
		G4cout << "Step at r0,r = (" << r0/m << ", " << r/m << ") m, P:" << pname << " (" << E/MeV << " MeV)" << G4endl;
		G4cout << " + TrackID          : " << step->GetTrack()->GetTrackID() << G4endl;
		G4cout << " + ProcessName      : " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() << G4endl;
		G4cout << " + ProcessType      : " << G4VProcess::GetProcessTypeName(step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType()) << G4endl;
		G4cout << " + GetProcessSubType: " << step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType() << G4endl;

		G4int nSecAtRest = fpSteppingManager->GetfN2ndariesAtRestDoIt();
		G4int nSecAlong  = fpSteppingManager->GetfN2ndariesAlongStepDoIt();
		G4int nSecPost   = fpSteppingManager->GetfN2ndariesPostStepDoIt();
		G4int nSecTotal  = nSecAtRest+nSecAlong+nSecPost;
		G4TrackVector* secVec = fpSteppingManager->GetfSecondary();

		if(nSecTotal>0) {
		G4cout << " + Secondaries: " << std::setw(2) << nSecTotal
		       << " (rest="   << std::setw(2) << nSecAtRest
		       << ", along=" << std::setw(2) << nSecAlong
		       << ", post="  << std::setw(2) << nSecPost << ")"
		       << G4endl;

			for(size_t lp1=(*secVec).size()-nSecTotal; lp1<(*secVec).size(); lp1++) {
				G4cout << "    > "
				       << std::setw(7) << (*secVec)[lp1]->GetDefinition()->GetParticleName()
				       << " (" << std::setw(5) << (*secVec)[lp1]->GetKineticEnergy()/MeV << " MeV) "
				       << " process: " << (*secVec)[lp1]->GetCreatorProcess()->GetProcessName()
				       << G4endl;
			}
		}
	}
	
	if(step->GetPostStepPoint()->GetStepStatus() == fWorldBoundary) {
		borderParticlesCounter++;
		if(is_neutrino(step->GetTrack()->GetParticleDefinition())) borderNeutrinosCounter++;
		
		if(!p_quiet){G4cout << "World boundary step at r1>r2 = (" << r0/m << " > " << r/m << ") m, P:" << pname << " (" << E/MeV << " MeV)" << G4endl;}
		hgr->addParticle(step->GetTrack());
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
