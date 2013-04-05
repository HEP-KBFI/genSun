#ifndef SunSteppingAction_h
#define SunSteppingAction_h

#include "G4UserSteppingAction.hh"

class G4Step;

class SunSteppingAction : public G4UserSteppingAction {
	bool verbose;
	
	public:
		SunSteppingAction(bool verbose=true);
		void UserSteppingAction(const G4Step* step);
};

#endif
