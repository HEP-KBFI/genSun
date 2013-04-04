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

// ---------------------------------------------------------------------
//                    Argument parser settings
// ---------------------------------------------------------------------
#include <argp.h>

const char* argp_program_version = "SolNuGeant";
const argp_option argp_options[] = {
	{"runs",     'n',    "runs",      0,   "Number of runs.", 0},
	{"vis",      'v',         0,      0,   "Enable visual mode.", 0},
	{0, 0, 0, 0, 0, 0} // terminates the array
};

int  p_runs = 1; // number of runs. Default: 1
bool p_vis  = false; // go to visual mode. Default: false
error_t argp_parser(int key, char *arg, struct argp_state *state) {
	switch(key) {
		case 'n':
			p_runs = std::atoi(arg);
			G4cout << "Setting number of runs to " << p_runs << G4endl;
			break;
		case 'v':
			p_vis = true;
			G4cout << "Enabling visual mode!" << G4endl;
			break;
		default:
			//G4cout << "Unknonwn key: " << key << G4endl;
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

const argp argp_argp = {
	argp_options,
	&argp_parser,
	"<channel id> <DM mass [GeV]>",
	"DM annihilation simulation",
	0, 0, 0
};

// ---------------------------------------------------------------------
//                                MAIN
// ---------------------------------------------------------------------
void go_visual(int argc, char* argv[]);
G4VUserPrimaryGeneratorAction* get_primary_generator_action(G4int channel, G4double dm_mass);

int main(int argc, char * argv[]) {
	// Parse the arguments
	int argp_index;
	argp_parse(&argp_argp, argc, argv, 0, &argp_index, 0);
	
	if(argp_index+2 != argc) {
		G4cerr << "Too many or too few arguments!" << G4endl;
		G4cout << " > argp_index = " << argp_index << G4endl;
		G4cout << " > argc = " << argc << G4endl;
		for(int i=0; i<argc; i++) {
			G4cout << " > argv[" << i << "] = `" << argv[i] << "`" << G4endl;
		}
		return 0;
	}
	G4int channel = atoi(argv[argp_index+0]);
	G4double dm_mass = atof(argv[argp_index+1]) * GeV;
	G4cout << "Simulating " << channel << " with E=" << dm_mass/GeV << " GeV for " << p_runs << " runs." << G4endl;
	
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
	
	//runManager->SetUserAction(new DMPrimaryGeneratorAction(13, -13, 1*GeV)); // particle gun
	G4VUserPrimaryGeneratorAction* primaryGeneratorAction = get_primary_generator_action(channel, dm_mass);
	if(primaryGeneratorAction == NULL) {
		G4cerr << "No generator!" << G4endl;
		return -1;
	} else {
		runManager->SetUserAction(primaryGeneratorAction);
	}
	
	//runManager->SetUserAction(new NeutrinoStackingAction(&h)); // hook for histogramming
	runManager->SetUserAction(new MyUserRunAction);
	runManager->Initialize(); // initialize G4 kernel

	if(p_vis) go_visual(argc, argv);
	else {
		G4cout << "Running for " << p_runs << G4endl;
		runManager->BeamOn(p_runs);
	}

	// job termination
	delete runManager;
	return 0;
}

G4VUserPrimaryGeneratorAction* get_primary_generator_action(G4int channel, G4double dm_mass) {
	G4VUserPrimaryGeneratorAction* generatorAction = NULL;
	
	switch(channel) {
		// Pythia8
		case 17: case -17: G4cout << "Simulating taus" << G4endl;
			G4cout << "Pythia not implemented!" << G4endl;
			break;
		
		// Geant4 only, particle + antiparticle
		case 11: case -11: G4cout << "Simulating electrons" << G4endl;
		case 13: case -13: G4cout << "Simulating muons" << G4endl;
			G4cout << "Standard particle generation (particle+antiparticle)" << G4endl;
			generatorAction = new DMPrimaryGeneratorAction(channel, -channel, dm_mass);
			break;
		
		// Geant4 only, particle = antiparticle
		case 22: G4cout << "Simulating gammas" << G4endl;
			G4cout << "Standard particle generation (same particle)" << G4endl;
			generatorAction = new DMPrimaryGeneratorAction(channel, channel, dm_mass);
			break;
		
		case 12: case 14: case 18: G4cout << "Simulating neutrinos" << G4endl;
		case -12: case -14: case -18: G4cout << "Simulating antineutrinos" << G4endl;
			G4cout << "Neutrinos not implemented." << G4endl;
			break;
		
		default:
			G4cout << "Bad channel: " << channel << G4endl;
			break;
	}
	
	return generatorAction;
}

void go_visual(int argc, char* argv[]) {
#ifdef G4VIS_USE
	// get the pointer to the UI manager and set verbosities
	G4UImanager* UI = G4UImanager::GetUIpointer();
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
