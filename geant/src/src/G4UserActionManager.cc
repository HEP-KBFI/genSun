#include "globals.hh"
#include "G4RunManager.hh"
#include "G4UserRunAction.hh"
#include "G4UserEventAction.hh"

#include "G4UserActionManager.hh"

extern bool p_quiet;

class GeneralUserRunAction : public G4UserRunAction {
	std::set<G4UserRunAction*> as;
	public:
		void addAction(G4UserRunAction* a) {as.insert(a);}
		void BeginOfRunAction(const G4Run* run) {for(auto it = as.begin(); it != as.end(); ++it) {(*it)->BeginOfRunAction(run);}}
		void EndOfRunAction(const G4Run* run) {for(auto it = as.begin(); it != as.end(); ++it) {(*it)->EndOfRunAction(run);}}
};

GeneralUserRunAction * g_runaction;

G4UserActionManager::G4UserActionManager(G4RunManager* rm) : runmanager(rm) {
	g_runaction = new GeneralUserRunAction;
	runmanager->SetUserAction(g_runaction);
}

G4UserActionManager::~G4UserActionManager() {}

void G4UserActionManager::addUserAction(G4UserRunAction* a) {
	g_runaction->addAction(a);
}

void G4UserActionManager::addUserAction(G4UserEventAction* a) {
	//
}

void G4UserActionManager::addAction(void* a) {
	actions.insert(a);
}
