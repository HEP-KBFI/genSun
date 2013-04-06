#ifndef SunSteppingAction_h
#define SunSteppingAction_h

#include "G4UserSteppingAction.hh"
#include "StatisticsRunAction.hh"

class G4Step;

class SunSteppingAction :
	public G4UserSteppingAction,
	public StatisticsRunActionTarget {
	
	bool verbose;
	
	G4int borderParticlesCounter, borderNeutrinosCounter;
	std::vector<G4int> borderParticles, borderNeutrinos;
	
	public:
		SunSteppingAction(bool verbose=true);
		void UserSteppingAction(const G4Step* step);
		
		void onBeginRun(const G4Run* run);
		void onEndRun(const G4Run* run);
		void statistics();
};

#endif
