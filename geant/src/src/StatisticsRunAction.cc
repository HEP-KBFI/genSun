#include "globals.hh"
#include "G4Run.hh"

#include "StatisticsRunAction.hh"

void StatisticsRunAction::BeginOfRunAction(const G4Run* run) {
	G4cout << "Starting run: ID = " << run->GetRunID() << G4endl;
	
	for(std::vector<StatisticsRunActionTarget*>::iterator it = targets.begin(); it != targets.end(); it++) {
		(*it)->onBeginRun(run);
	}
}

void StatisticsRunAction::addTarget(StatisticsRunActionTarget* target) {
	targets.push_back(target);
}
