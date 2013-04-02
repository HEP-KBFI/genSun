#include "DMPhysicsList.hh"

#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"

DMPhysicsList::DMPhysicsList() : G4VUserPhysicsList() {}
DMPhysicsList::~DMPhysicsList() {}

void DMPhysicsList::ConstructParticle() {
	// In this method, static member functions should be called
	// for all particles which you want to use.
	// This ensures that objects of these particle types will be
	// created in the program.

	G4Geantino::GeantinoDefinition();
	G4ChargedGeantino::ChargedGeantinoDefinition();
}

void DMPhysicsList::ConstructProcess() {
	// Define transportation process
	AddTransportation();
}
