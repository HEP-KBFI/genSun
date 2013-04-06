#include "globals.hh"
#include "G4Run.hh"

#include "StatisticsRunAction.hh"

extern bool p_quiet;

void StatisticsRunAction::BeginOfRunAction(const G4Run* run) {
	G4cout << "Starting run: ID = " << run->GetRunID() << G4endl;
	
	for(std::vector<StatisticsRunActionTarget*>::iterator it = targets.begin(); it != targets.end(); it++) {
		(*it)->onBeginRun(run);
	}
}

void StatisticsRunAction::EndOfRunAction(const G4Run* run) {
	if(p_quiet){G4cout << "Ending run: ID = " << run->GetRunID() << G4endl;}
	for(std::vector<StatisticsRunActionTarget*>::iterator it = targets.begin(); it != targets.end(); it++) {
		(*it)->onEndRun(run);
	}
}

void StatisticsRunAction::addTarget(StatisticsRunActionTarget* target) {
	targets.push_back(target);
}
