#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "SunDetectorConstruction.hh"
#include "DMPhysicsList.hh"
#include "DMPrimaryGeneratorAction.hh"

int main(int argc, char * argv[]) {
	// construct the default run manager
	G4RunManager* runManager = new G4RunManager;

	// set mandatory initialization classes
	runManager->SetUserInitialization(new SunDetectorConstruction);
	runManager->SetUserInitialization(new DMPhysicsList);

	// set mandatory user action class
	runManager->SetUserAction(new DMPrimaryGeneratorAction);

	// initialize G4 kernel
	runManager->Initialize();

#ifdef G4VIS_USE
	// Initialize visualization
	G4VisManager* visManager = new G4VisExecutive;
	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	// G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize();
#endif

	// get the pointer to the UI manager and set verbosities
	G4UImanager* UI = G4UImanager::GetUIpointer();
	if (argc!=1) {
		// batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UI->ApplyCommand(command+fileName);
	} else {
		// interactive mode : define UI session
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
		UI->ApplyCommand("/control/execute vis.mac"); 
#endif
		ui->SessionStart();
		delete ui;
	}

	// job termination
#ifdef G4VIS_USE
	delete visManager;
#endif
	delete runManager;
	return 0;
}