#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "SunDetectorConstruction.hh"
#include "DMPhysicsList.hh"
#include "DMPrimaryGeneratorAction.hh"

int main() {
	// construct the default run manager
	G4RunManager* runManager = new G4RunManager;

	// set mandatory initialization classes
	runManager->SetUserInitialization(new SunDetectorConstruction);
	runManager->SetUserInitialization(new DMPhysicsList);

	// set mandatory user action class
	runManager->SetUserAction(new DMPrimaryGeneratorAction);

	// initialize G4 kernel
	runManager->Initialize();

	// get the pointer to the UI manager and set verbosities
	G4UImanager* UI = G4UImanager::GetUIpointer();
	UI->ApplyCommand("/run/verbose 1");
	UI->ApplyCommand("/event/verbose 1");
	UI->ApplyCommand("/tracking/verbose 1");

	// start a run
	int numberOfEvent = 3;
	runManager->BeamOn(numberOfEvent);

	// job termination
	delete runManager;
	return 0;
}
