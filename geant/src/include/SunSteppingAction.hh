#ifndef SunSteppingAction_h
#define SunSteppingAction_h

#include "G4UserSteppingAction.hh"

class G4Step;

class SunSteppingAction : public G4UserSteppingAction {
	SunSteppingAction* histogram;
	
	public:
		SunSteppingAction();
		void UserSteppingAction(const G4Step* step);
};

#endif
