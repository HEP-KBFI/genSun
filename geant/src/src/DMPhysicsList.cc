#include "DMPhysicsList.hh"

//#include "G4Geantino.hh"
//#include "G4ChargedGeantino.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4ExcitedDeltaConstructor.hh"
#include "G4ExcitedLambdaConstructor.hh"
#include "G4ExcitedMesonConstructor.hh"
#include "G4ExcitedNucleonConstructor.hh"
#include "G4ExcitedXiConstructor.hh"
#include "G4ShortLivedConstructor.hh"

DMPhysicsList::DMPhysicsList() : G4VUserPhysicsList() {}
DMPhysicsList::~DMPhysicsList() {}

void DMPhysicsList::ConstructParticle() {
	// In this method, static member functions should be called
	// for all particles which you want to use.
	// This ensures that objects of these particle types will be
	// created in the program.

	//G4Geantino::GeantinoDefinition();
	//G4ChargedGeantino::ChargedGeantinoDefinition();
	G4BosonConstructor::ConstructParticle();
	G4BaryonConstructor::ConstructParticle();
	G4IonConstructor::ConstructParticle();
	G4MesonConstructor::ConstructParticle();
	G4LeptonConstructor::ConstructParticle();
	G4ShortLivedConstructor::ConstructParticle();
	//G4ExcitedDeltaConstructor::ConstructParticle();
	//G4ExcitedLambdaConstructor::ConstructParticle();
	//G4ExcitedMesonConstructor::ConstructParticle();
	//G4ExcitedNucleonConstructor::ConstructParticle();
	//G4ExcitedSigmaConstructor::ConstructParticle();
	//G4ExcitedXiConstructor::ConstructParticle();
}

void DMPhysicsList::ConstructProcess() {
	// Define transportation process
	AddTransportation();
}
