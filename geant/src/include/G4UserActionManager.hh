#ifndef G4UserActionManager_h
#define G4UserActionManager_h

#include <set>

class G4RunManager;
class G4UserRunAction;
class G4UserEventAction;

class G4UserActionManager {
	public:
		G4UserActionManager(G4RunManager* rm);
		~G4UserActionManager();
		
		void addUserAction(G4UserRunAction* a);
		void addUserAction(G4UserEventAction* a);
	
	private:
		G4RunManager* runmanager;
		std::set<void*> actions;
		
		void addAction(void* a);
};

#endif
