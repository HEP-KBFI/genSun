#ifndef SunSteppingAction_h
#define SunSteppingAction_h

#include "G4UserSteppingAction.hh"
#include "G4UserRunAction.hh"

class DMRootHistogrammer;
class G4Step;

class SunSteppingAction :
	public G4UserSteppingAction,
	public G4UserRunAction {
	
	DMRootHistogrammer * hgr;
	
	G4int borderParticlesCounter, borderNeutrinosCounter;
	std::vector<G4int> borderParticles, borderNeutrinos;
	
	public:
		SunSteppingAction(DMRootHistogrammer* hgr);
		void UserSteppingAction(const G4Step* step);
		
		void BeginOfRunAction(const G4Run* run);
		void EndOfRunAction(const G4Run* run);
		void statistics();
};

#endif
