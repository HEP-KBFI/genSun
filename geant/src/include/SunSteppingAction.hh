#ifndef SunSteppingAction_h
#define SunSteppingAction_h

#include "G4UserSteppingAction.hh"
#include "G4UserRunAction.hh"

class G4Step;

class SunSteppingAction :
	public G4UserSteppingAction,
	public G4UserRunAction {
	
	bool verbose;
	
	G4int borderParticlesCounter, borderNeutrinosCounter;
	std::vector<G4int> borderParticles, borderNeutrinos;
	
	public:
		SunSteppingAction(bool verbose=true);
		void UserSteppingAction(const G4Step* step);
		
		void BeginOfRunAction(const G4Run* run);
		void EndOfRunAction(const G4Run* run);
		void statistics();
};

#endif
