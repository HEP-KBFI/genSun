#ifndef DMPhysicsList_h
#define DMPhysicsList_h 1

#include "G4VUserPhysicsList.hh"

/// Physics list with geantino and charged geantino only

class DMPhysicsList: public G4VUserPhysicsList {
	public:
		DMPhysicsList();
		~DMPhysicsList();

	protected:
		// Construct particle and physics process
		void ConstructParticle();
		void ConstructProcess();
};

#endif







