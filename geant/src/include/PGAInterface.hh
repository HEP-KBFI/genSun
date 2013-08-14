#ifndef PGAInterface_h
#define PGAInterface_h

#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class PGAInterface : public G4VUserPrimaryGeneratorAction {
	public:
		PGAInterface() : G4VUserPrimaryGeneratorAction() {};
		virtual const char* getName()=0;
};

#endif


