#ifndef StatisticsRunAction_h
#define StatisticsRunAction_h

#include "G4UserRunAction.hh"

class G4Run;

class StatisticsRunActionTarget {
	public:
		virtual void onBeginRun(const G4Run* run)=0;
};

class StatisticsRunAction : public G4UserRunAction {
	std::vector<StatisticsRunActionTarget*> targets;
	
	public:
		//G4Run* GenerateRun();
		void BeginOfRunAction(const G4Run* run);
		//void EndOfRunAction(const G4Run*);
		
		void addTarget(StatisticsRunActionTarget* target);
};

#endif
