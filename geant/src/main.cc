#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"

#include "SunDetectorConstruction.hh"
#include "SunSteppingAction.hh"
#include "DMPhysicsList.hh"
#include "DMPrimaryGeneratorAction.hh"
#include "DMPythiaPGA.hh"
#include "NeutrinoStackingAction.hh"
#include "NeutrinoHistogram.hh"
#include "StatisticsRunAction.hh"
#include "G4UserActionManager.hh"

#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"

// ---------------------------------------------------------------------
//                    Argument parser settings
// ---------------------------------------------------------------------
#include <argp.h>

const char* argp_program_version = "SolNuGeant";
const argp_option argp_options[] = {
	{"runs",     'n',    "runs",      0,   "Number of runs.", 0},
	{"vis",      'v',         0,      0,   "Enable visual mode.", 0},
	{"quiet",    'q',         0,      0,   "Reduce verbosity as much as possible.", 0},
	{"ofile",    'o',   "ofile",      0,   "Output root file.", 0},
	{"physics",  'p', "physics",      0, "Specify the physics (FULL, TRANS, VAC, VACTRANS). If not specified, FULL is used.", 0},
	{0, 0, 0, 0, 0, 0} // terminates the array
};

int  p_runs = 1; // number of runs. Default: 1
bool p_vis  = false; // go to visual mode. Default: false
bool p_quiet = false; // reduce verbosity. Default: false
bool p_vacuum = false; // use vacuum instead of sun. Default: false
bool p_trans = false; // use only translation physics. Default: false
G4String p_ofile = "output.root";
error_t argp_parser(int key, char *arg, struct argp_state *state) {
	switch(key) {
		case 'n':
			p_runs = std::atoi(arg);
			//G4cout << "Setting number of runs to " << p_runs << G4endl;
			break;
		case 'v':
			p_vis = true;
			//G4cout << "Enabling visual mode!" << G4endl;
			break;
		case 'q':
			p_quiet = true;
			//G4cout << "Silence! I kill you!" << G4endl;
			break;
		case 'o':
			p_ofile = arg;
			//G4cout << "Setting ofile to " << p_ofile << G4endl;
			break;
		case 'p':
			if(strcmp(arg, "VAC") == 0) {
				p_vacuum = true;
			} else if(strcmp(arg, "TRANS") == 0) {
				p_trans = true;
			} else if(strcmp(arg, "VACTRANS") == 0) {
				p_vacuum = true;
				p_trans = true;
			}
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
	
	G4cout << "G4-sim: "
	       <<    "ch:" << channel
	       << " | runs:" << p_runs
	       << " | m_dm[GeV]:" << dm_mass/GeV
	       << " | mat:" << (p_vacuum ? "VAC" : "SUN")
	       << " | phys:" << (p_trans ? "TRANS" : "QGSP_BERT")
	       << G4endl;
	
	// Start setting up Geant4
	// Start constructing the run manager
	G4RunManager* runManager = new G4RunManager;
	runManager->SetVerboseLevel( p_quiet ? 0 : 1 );
	
	// detector
	runManager->SetUserInitialization(new SunDetectorConstruction(
		p_vacuum ? 10*km : 10*m,
		p_vacuum
	));
	
	// create the physics list (using a factory)
	//G4VModularPhysicsList* physlist;
	G4VUserPhysicsList * physlist;
	if(!p_trans) {
		G4PhysListFactory factory;
		physlist = factory.GetReferencePhysList("QGSP_BERT");
		physlist->SetVerboseLevel( p_quiet ? 0 : 1 );
	} else {
		physlist = new DMPhysicsList;
	}
	runManager->SetUserInitialization(physlist); // physics
	
	G4VUserPrimaryGeneratorAction* primaryGeneratorAction = get_primary_generator_action(channel, dm_mass);
	if(primaryGeneratorAction == NULL) {
		G4cerr << "No generator!" << G4endl;
		return -1;
	} else {
		runManager->SetUserAction(primaryGeneratorAction);
	}
	
	// create and add actions
	NeutrinoHistogram* h = new NeutrinoHistogram(channel, dm_mass);
	NeutrinoStackingAction* neutrino_stacking_action = new NeutrinoStackingAction(h);
	SunSteppingAction* sun_stepping_action = new SunSteppingAction(!p_quiet);
	
	G4UserActionManager* actionManager = new G4UserActionManager(runManager);
	actionManager->addUserAction((G4UserRunAction*)sun_stepping_action);
	
	runManager->SetUserAction(neutrino_stacking_action); // hook for histogramming
	runManager->SetUserAction((G4UserSteppingAction*)sun_stepping_action);
	
	if(!p_quiet){G4cout << "===========================   BEGIN  INIT   ===========================" << G4endl;}
	runManager->Initialize(); // initialize G4 kernel
	if(!p_quiet){G4cout << "===========================    END   INIT   ===========================" << G4endl;}

	if(p_vis) go_visual(argc, argv);
	else {
		G4cout << "Starting simulation: runs = " << p_runs << G4endl;
		runManager->BeamOn(p_runs);
	}
	h->countRuns(p_runs);
	h->write(p_ofile);
	
	if(!p_quiet){sun_stepping_action->statistics();}

	// job terminationi
	if(!p_quiet){G4cout << "Deconstructing..." << G4endl;}
	delete runManager;
	G4cout << "Closing program!" << G4endl;
	return 0;
}

enum generator_mode {m_pythia, m_ppbar, m_2p};
G4VUserPrimaryGeneratorAction* get_primary_generator_action(G4int channel, G4double dm_mass) {
	generator_mode mode;
	switch(channel) {
		// Quarks and gluons
		case 1: case -1: case 2: case -2:
		case 3: case -3: case 4: case -4:
		case 5: case -5: case 6: case -6:
			if(!p_quiet){G4cout << "Particle: quark/antiquark" << G4endl;} mode = m_pythia; break;
		
		// Gamma
		case 22:
			if(!p_quiet){G4cout << "Particle: gamma" << G4endl;} mode = m_2p; break;
		
		// W, Z and H bosons
		case 23:
			if(!p_quiet){G4cout << "Particle: Z boson" << G4endl;} mode = m_pythia; break;
		case 24: case -24:
			if(!p_quiet){G4cout << "Particle: W +/- boson" << G4endl;} mode = m_pythia; break;
		case 25:
			if(!p_quiet){G4cout << "Particle: H boson" << G4endl;} mode = m_pythia; break;
		
		// Leptons
		case 11: case -11:
			if(!p_quiet){G4cout << "Particle: electron/positron" << G4endl;} mode = m_ppbar; break;
		case 13: case -13:
			if(!p_quiet){G4cout << "Particle: muon/antimuon" << G4endl;} mode = m_ppbar; break;
		case 15: case -15:
			if(!p_quiet){G4cout << "Particle: tau/antitau" << G4endl;} mode = m_pythia; break;
		
		// Neutrinos
		case 12: case -12: case 14: case -14: case 16: case -16:
			if(!p_quiet){G4cout << "Particle: neutrino" << G4endl;} mode = m_ppbar; break;
		
		default:
			G4cout << "Bad channel: " << channel << G4endl;
			break;
	}
	
	G4VUserPrimaryGeneratorAction* generatorAction = NULL;
	switch(mode) {
		case m_pythia:
			if(!p_quiet){G4cout << "Going for Pythia8!" << G4endl;}
			generatorAction = new DMPythiaPGA(channel, dm_mass);
			break;
		
		case m_ppbar:
			if(!p_quiet){G4cout << "Standard particle generation (particle+antiparticle)" << G4endl;}
			generatorAction = new DMPrimaryGeneratorAction(channel, -channel, dm_mass);
			break;
		
		case m_2p:
			if(!p_quiet){G4cout << "Standard particle generation (two identical particles)" << G4endl;}
			generatorAction = new DMPrimaryGeneratorAction(channel, channel, dm_mass);
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
