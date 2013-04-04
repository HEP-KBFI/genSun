#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"

#include "SunDetectorConstruction.hh"
//#include "DMPhysicsList.hh"
#include "DMPrimaryGeneratorAction.hh"
#include "NeutrinoStackingAction.hh"
#include "NeutrinoHistogram.hh"

#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"

class MyUserRunAction : public G4UserRunAction {
	public:
		//G4Run* GenerateRun();
		void BeginOfRunAction(const G4Run* run);
		//void EndOfRunAction(const G4Run*);
};

void MyUserRunAction::BeginOfRunAction(const G4Run* run) {
	G4cout << "Starting run. RunID=" << run->GetRunID() << G4endl;
}

int main(int argc, char * argv[]) {
	// Start setting up Geant4
	NeutrinoHistogram h; // create the histogrammer
	
	// Start constructing the run manager
	G4RunManager* runManager = new G4RunManager;
	runManager->SetUserInitialization(new SunDetectorConstruction); // detector
	
	// create the physics list (using a factory)
	G4VModularPhysicsList* physlist;
	//physlist = new DMPhysicsList;
	G4PhysListFactory factory;
	physlist = factory.GetReferencePhysList("QGSP_BERT");
	physlist->SetVerboseLevel(3);
	
	runManager->SetUserInitialization(physlist); // physics
	runManager->SetUserAction(new DMPrimaryGeneratorAction(13, -13, 1*GeV)); // particle gun
	runManager->SetUserAction(new NeutrinoStackingAction(&h)); // hook for histogramming
	runManager->SetUserAction(new MyUserRunAction);
	runManager->Initialize(); // initialize G4 kernel

	// get the pointer to the UI manager and set verbosities
	G4UImanager* UI = G4UImanager::GetUIpointer();
	if (argc!=1) {
		// batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UI->ApplyCommand(command+fileName);
	} else {
#ifdef G4VIS_USE
		// Initialize visualization
		G4VisManager* visManager = new G4VisExecutive;
		// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
		// G4VisManager* visManager = new G4VisExecutive("Quiet");
		visManager->Initialize();
		// interactive mode : define UI session
		G4UIExecutive* ui = new G4UIExecutive(argc, argv);

		UI->ApplyCommand("/control/execute vis.mac"); 

		ui->SessionStart();
		delete ui;
		delete visManager;
#else
		G4err << "No visalization compiled!" << G4endl;
#endif
	}

	// job termination
	delete runManager;
	return 0;
}
