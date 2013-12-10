#include <ctime>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "CLHEP/Random/Random.h"

#include "PGAInterface.hh"
#include "SunDetectorConstruction.hh"
#include "SunSteppingAction.hh"
#include "DMPhysicsList.hh"
#include "Geant4PGA.hh"
#include "DMPythiaPGA.hh"
#include "NeutrinoStackingAction.hh"
#include "NeutrinoHistogram.hh"
#include "G4UserActionManager.hh"
#include "DMRootHistogrammer.hh"
#include "ParentTrackingAction.hh"

#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"

#include "G4Neutron.hh"
#include "G4AntiNeutron.hh"
#include <G4DecayTable.hh>

// ---------------------------------------------------------------------
//                    Argument parser settings
// ---------------------------------------------------------------------
// The GNU `argp` is used to conveniently parse command line arguments.
// The documentation: http://www.gnu.org/software/libc/manual/html_node/Argp.html
#include <argp.h>
const char* argp_program_version = "solnugeant";

// Codes for options without the short name.
#define PC_NDC  1001
#define PC_TRK  1002
#define PC_TRV  1003
#define PC_SEED 1004
#define PC_RAD  1005
#define PC_BAR  1006
#define PC_KINE 1007

// Program's arguments - an array of option specifiers
// name, short name, arg. name, flags, doc, group
const argp_option argp_options[] = {
	{0, 0, 0, 0, "General options:", 0},
	{"quiet", 'q',       0, 0, "reduce verbosity as much as possible", 0},
	{"ofile", 'o', "OFILE", 0, "write spectra to OFILE; default is output.root", 0},
	{"vis",   'v',       0, 0,
		"open the GUI instead of running the simulation", 0},

	{"seed", PC_SEED, "SEED", 0,
		"set the seed for the random generators; if this is not"
		" specified, time(0) is used)", 0},
	{"runs",     'n',    "N", 0, "run the simulation N times", 0},
	{"unit",     'u', "UNIT", 0,
		"explicitly specify the unit of the <DM mass>; supported options"
		" are 'G' for GeV and 'M' for MeV; GeV is the default", 0},
	{"kinetic", PC_KINE, 0, 0,
		"use the provided energy as kinetic (particle gun only).", 0},
	{"bar",  PC_BAR,      0, 0,
		"use the antiparticle instead (i.e. the negative PDG code);"
		" useful for single particle runs", 0},

	{0, 0, 0, 0, "Options for tweaking the physics:", 2},
	{"physics", 'p', "PHYSICS", 0, "specify the physics list;"
		" 'TRANS' refers to the custom translation-only physics list;"
		" otherwise the specified Geant4 reference physics list is used;"
		" the default list is 'QGSP_BERT'", 2},
	{"creator", 'c', "CREATOR", 0,
		"specify how the particles are created; possible values are"
		" 'PYTHIA8' (default), 'P8single', 'G4' or 'G4single'", 2},

	{"radius",        PC_RAD,       "R", 0,
		"set the radius of the world in meters; the default is 1000 meters;"
		" in vacuum the value is multiplied by factor of 1 million", 2},
	{"material",         'm',     "MAT", 0,
		"specify the material of the world; possible values are 'SUN'"
		" (default), 'SUNFULL' and 'VAC'", 2},
	{"short-neutron", PC_NDC,  "on/off", 0,
		"enable/disable short-lived neutrons", 2},
	{"track-kill",    PC_TRK,  "on/off", 0,
		"enable/disable the killing of low energy tracks", 2},
	{"track-verbose", PC_TRV,         0, 0,
		"print out the created Geant4 tracks", 2},

	{0, 0, 0, 0, "Other:", -1},
	{0, 0, 0, 0, 0, 0} // terminates the array
};

// Global variables set with command line arguments that configure the
// simulation. The initial value is the default value. It might then be
// later modified by the `argp_parser` function, which is called by the
// argp argument parser.
int  p_runs = 1; // number of runs
bool p_vis  = false; // go to visual mode (i.e. open the GUI instead)
bool p_quiet = false; // maximally reduce verbosity if true
G4String p_phys = "QGSP_BERT"; // physics list
enum {MAT_VAC, MAT_SUN, MAT_SUNFULL} p_mat = MAT_SUN; // material of the world
enum {CRE_PYTHIA8, CRE_PYTHIA8SINGLE, CRE_GEANT4, CRE_GEANT4SINGLE} p_cre = CRE_PYTHIA8; // particle creator
enum {NDC_UNDEF, NDC_SHORT, NDC_LONG} p_ndc = NDC_UNDEF; // neutron lifetime flag
enum {TRK_UNDEF, TRK_ON, TRK_OFF} p_trk = TRK_UNDEF; // killing low energy tracks
bool p_bar = false; // use the antiparticle (negative PDG code)
bool p_kinetic = false; // use provided energy as kinetic
bool p_trv = false; // if true, print G4 tracks
int p_seed = 0; // seed value; p_seed==0 => seed=time(0)
G4double p_radius = 1000.0; // world radius in meters
G4double p_unit = GeV;
G4String p_ofile = "output.root";

// Functions to get some derived values of parameters (e.g. enum -> string)
inline const char * p_mat_str() {
	// return the string representation of `p_mat`
	switch(p_mat) {
		case MAT_SUN:
			return "SUN";
			break;
		case MAT_VAC:
			return "VACUUM";
			break;
		case MAT_SUNFULL:
			return "SUNFULL";
			break;
	}
	G4cerr << "ERROR (material_string): bad material ID (`" << (int)m << "`)" << G4endl;
	return "ERROR";
}

inline bool p_vacuum() {
	// return true if `p_mat` is a vacuum
	return p_mat == MAT_VAC;
}

inline unsigned int p_mat_fractions() {
	// return the number of fraction corresponding to `p_mat`
	switch(p_mat) {
		case MAT_VAC:
			return 0;
			break;
		case MAT_SUN:
			return 2;
			break;
		case MAT_SUNFULL:
			return 10;
			break;
	}
	G4cerr << "ERROR (material_string): bad material ID (`" << (int)m << "`)" << G4endl;
	return 0;
}

// Argument parser callback called by argp
error_t argp_parser(int key, char *arg, struct argp_state *state) {
	switch(key) {
		case 'n':
			p_runs = std::atoi(arg);
			break;
		case 'v':
			p_vis = true;
			break;
		case 'q':
			p_quiet = true;
			break;
		case 'o':
			p_ofile = arg;
			break;
		case 'p':
			p_phys = arg;
			break;
		case 'm':
			if(strcmp(arg, "SUN") == 0) {
				p_mat = MAT_SUN;
			} else if(strcmp(arg, "VAC") == 0) {
				p_mat = MAT_VAC;
			} else if(strcmp(arg, "SUNFULL") == 0) {
				p_mat = MAT_SUNFULL;
			} else {
				G4cout << "Bad material: " << arg << G4endl;
				return ARGP_ERR_UNKNOWN;
			}
			break;
		case 'c':
			if(strcmp(arg, "PYTHIA8") == 0) {
				p_cre = CRE_PYTHIA8;
			} else if(strcmp(arg, "P8single") == 0) {
				p_cre = CRE_PYTHIA8SINGLE;
			} else if(strcmp(arg, "G4") == 0) {
				p_cre = CRE_GEANT4;
			} else if(strcmp(arg, "G4single") == 0) {
				p_cre = CRE_GEANT4SINGLE;
			} else {
				G4cout << "Bad creator: " << arg << G4endl;
				return ARGP_ERR_UNKNOWN;
			}
			break;
		case 'u':
			if(arg[0] == 'M') {
				p_unit = MeV;
			} else if(arg[0] == 'G') {
				p_unit = GeV;
			} else {
				G4cout << "Bad units: " << arg << G4endl;
				return ARGP_ERR_UNKNOWN;
			}
			break;
		case PC_BAR:
			p_bar = true;
			break;
		case PC_NDC:
			if(strcmp(arg, "on") == 0) {
				p_ndc = NDC_SHORT;
			} else if(strcmp(arg, "off") == 0) {
				p_ndc = NDC_LONG;
			} else {
				G4cout << "Bad value for --short-neutron: " << arg << G4endl;
				return ARGP_ERR_UNKNOWN;
			}
			break;
		case PC_TRK:
			if(strcmp(arg, "on") == 0) {
				p_trk = TRK_ON;
			} else if(strcmp(arg, "off") == 0) {
				p_trk = TRK_OFF;
			} else {
				G4cout << "Bad value for --track-kill: " << arg << G4endl;
				return ARGP_ERR_UNKNOWN;
			}
			break;
		case PC_TRV:
			p_trv = true;
			break;
		case PC_SEED:
			p_seed = std::atoi(arg);
			break;
		case PC_RAD:
			p_radius = std::atof(arg);
			break;
		case PC_KINE:
			p_kinetic = true;
			break;
		default:
			//G4cout << "Unknonwn key: " << key << G4endl;
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

// argp configuration structure passed to the `argp_parse` function
const argp argp_argp = {
	argp_options,
	&argp_parser,
	"<CHANNEL> <MASS>",
	"Simulate the annihilation of a dark matter particles in the Sun's core."
	"\n\n"
	"It is assumed that the DM has a mass MASS and decays solely into"
	" particles specified by CHANNEL (which has to be the PDG ID of one"
	" of the particles in the created particle-antiparticle pair)."
	" The spectra of the created neutrinos will be stored to a ROOT file.",
	0, 0, 0
};

// ---------------------------------------------------------------------
//                                MAIN
// ---------------------------------------------------------------------
void go_visual(int argc, char* argv[]);
PGAInterface* get_primary_generator_action(G4int channel, G4double dm_mass, int seedvalue);

#define PRINTVARINFO(name,val,info) G4cout << "$ " << (name) << " " << (val) << " [" << (info) << "]" << G4endl
#define PRINTVAR(name,val) G4cout << "$ " << (name) << " " << (val) << G4endl
#define PRINT(string) G4cout << "$ " << (string) << G4endl

int main(int argc, char * argv[]) {
	// parse the arguments
	int argp_index;
	argp_parse(&argp_argp, argc, argv, 0, &argp_index, 0);

	// the two positional arguments have to be handled manually
	if(argp_index+2 != argc) {
		G4cerr << "Too many or too few arguments!" << G4endl;
		G4cout << " > argp_index = " << argp_index << G4endl;
		G4cout << " > argc = " << argc << G4endl;
		for(int i=0; i<argc; i++) {
			G4cout << " > argv[" << i << "] = `" << argv[i] << "`" << G4endl;
		}
		return 0;
	}
	G4int channel = atoi(argv[argp_index+0]) * (p_bar ? -1 : 1);
	G4double dm_mass = atof(argv[argp_index+1]) * p_unit;

	// start of the simulation
	G4cout << "--- Geant4 simulation of solar neutrinos ---" << G4endl;
	PRINTVAR("channel", channel);
	PRINTVAR("runs", p_runs);
	PRINTVARINFO("m_dm", dm_mass/GeV, "GeV");
	PRINTVAR("material", p_mat_str());
	PRINTVAR("physics", p_phys);
	PRINTVAR("unit", p_unit/eV);

	// Decide the random seed
	int seedvalue = (p_seed==0)? std::time(0) : p_seed;
	if(!p_quiet){G4cout << "Seed: " << seedvalue << G4endl;}

	// Start setting up Geant4
	// Start constructing the run manager
	G4RunManager* runManager = new G4RunManager;
	runManager->SetVerboseLevel( p_quiet ? 0 : 1 );

	// detector
	G4double final_radius = p_vacuum() ? p_radius*1e6*m : p_radius*m;
	PRINTVARINFO("radius", final_radius/m, "m");
	runManager->SetUserInitialization(new SunDetectorConstruction(
		final_radius,
		p_mat_fractions()
	));

	// set the physics list; for translation a custom list is used,
	// otherwise a Geant4 reference physics list is used
	G4VUserPhysicsList * physlist;
	if(p_phys == "TRANS") {
		physlist = new DMPhysicsList;
	} else {
		G4PhysListFactory factory;
		physlist = factory.GetReferencePhysList(p_phys);
		if(physlist == 0) {
			G4cerr << "Bad physics list `" << p_phys << "`!" << G4endl;
			return 1;
		}
		physlist->SetVerboseLevel(p_quiet ? 0 : 1);
	}
	runManager->SetUserInitialization(physlist); // physics

	PGAInterface* primaryGeneratorAction = get_primary_generator_action(channel, dm_mass, seedvalue);
	if(primaryGeneratorAction == NULL) {
		G4cerr << "No generator!" << G4endl;
		return -1;
	} else {
		runManager->SetUserAction(primaryGeneratorAction);
	}

	// stacking action for energy cutoff
	bool b_track_killing = p_trk==TRK_ON || (p_trk==TRK_UNDEF && !p_vacuum());
	if(b_track_killing) {
		G4cout << "Track killing: enabled!" << G4endl;
		NeutrinoStackingAction* cutoff_stacking_action = new NeutrinoStackingAction(p_trv);
		runManager->SetUserAction(cutoff_stacking_action);
	} else {
		G4cout << "Track killing: disabled!" << G4endl;
	}

	// create the physics string
	char str_physics[50];
	sprintf(str_physics, "%s_%s_trk%s_%s",
	        p_mat_str(),
	        p_phys.c_str(),
	        b_track_killing ? "ON" : "OFF",
	        primaryGeneratorAction->getName()
	);
	//if(!p_quiet){G4cout << "Full physics string: " << str_physics << G4endl;}
	G4cout << "PHYS_STR=\"" << str_physics << "\"" << G4endl;

	if(p_kinetic) {
		dm_mass += G4ParticleTable::GetParticleTable()->FindParticle(channel)->GetPDGMass();
	}

	DMRootHistogrammer* hgr = new DMRootHistogrammer(channel, dm_mass, str_physics);
	SunSteppingAction* sun_stepping_action = new SunSteppingAction(hgr);
	G4UserActionManager* actionManager = new G4UserActionManager(runManager);
	actionManager->addUserAction((G4UserRunAction*)sun_stepping_action);
	runManager->SetUserAction((G4UserSteppingAction*)sun_stepping_action);
	runManager->SetUserAction(new ParentTrackingAction);

	if(!p_quiet){G4cout << "===========================   BEGIN  INIT   ===========================" << G4endl;}
	runManager->Initialize(); // initialize G4 kernel
	if(!p_quiet){G4cout << "===========================    END   INIT   ===========================" << G4endl;}

	// Set up the neutron. Antineutron is stable by default for some reason...
	// Also, if necessary, set neutron lifetime to a microsecond (forces decay in vacuum)
	G4AntiNeutron::AntiNeutron()->SetPDGStable(false);
	if(p_ndc == NDC_SHORT || (p_ndc == NDC_UNDEF && p_vacuum())) {
		G4Neutron::Neutron()->SetPDGLifeTime(1e-6*s);
		G4AntiNeutron::AntiNeutron()->SetPDGLifeTime(1e-6*s);
	}

	if(!p_quiet){
		G4cout << "Neutron: "
		       << "T=" << G4Neutron::Neutron()->GetPDGLifeTime()/s << " [s]"
		       << ", " << (G4Neutron::Neutron()->GetPDGStable() ? "stable" : "unstable")
		       << G4endl;
		G4Neutron::Neutron()->GetDecayTable()->DumpInfo();
		G4cout << "Antineutron: "
		       << "T=" << G4AntiNeutron::AntiNeutron()->GetPDGLifeTime()/s << " [s]"
		       << ", " << (G4AntiNeutron::AntiNeutron()->GetPDGStable() ? "stable" : "unstable")
		       << G4endl;
		G4AntiNeutron::AntiNeutron()->GetDecayTable()->DumpInfo();
	}

	// Information about Geant4 randomness
	CLHEP::HepRandom::setTheSeed(seedvalue);
	if(!p_quiet) {CLHEP::HepRandom::showEngineStatus();}

	// Start runs or go into visual mode
	if(p_vis) {
		go_visual(argc, argv);
	} else {
		G4cout << "Starting simulation: runs = " << p_runs << G4endl;
		runManager->BeamOn(p_runs);

		hgr->countEvent(p_runs);
		hgr->save(p_ofile);

		if(!p_quiet){sun_stepping_action->statistics();}
	}

	// job termination
	if(!p_quiet){G4cout << "Deconstructing..." << G4endl;}
	delete runManager;

	PRINT("closing");
	G4cout << "--- Closing Geant4 simulation! ---" << G4endl;
	return 0;
}

PGAInterface* get_primary_generator_action(G4int channel, G4double dm_mass, int seedvalue) {
	bool pythia_only = false;
	switch(channel) {
		// Quarks and gluons
		case 1: case -1: case 2: case -2:
		case 3: case -3: case 4: case -4:
		case 5: case -5: case 6: case -6:
			if(!p_quiet){G4cout << "Particle: quark/antiquark" << G4endl;} pythia_only = true; break;

		case 21:
			if(!p_quiet){G4cout << "Particle: gluon" << G4endl;} pythia_only = true; break;

		// Gamma
		case 22:
			if(!p_quiet){G4cout << "Particle: gamma" << G4endl;} break;

		// W, Z and H bosons
		case 23:
			if(!p_quiet){G4cout << "Particle: Z boson" << G4endl;} pythia_only = true; break;
		case 24: case -24:
			if(!p_quiet){G4cout << "Particle: W +/- boson" << G4endl;} pythia_only = true; break;
		case 25:
			if(!p_quiet){G4cout << "Particle: H boson" << G4endl;} pythia_only = true; break;

		// Leptons
		case 11: case -11:
			if(!p_quiet){G4cout << "Particle: electron/positron" << G4endl;} break;
		case 13: case -13:
			if(!p_quiet){G4cout << "Particle: muon/antimuon" << G4endl;} break;
		case 15: case -15:
			if(!p_quiet){G4cout << "Particle: tau/antitau" << G4endl;} break;

		// Neutrinos
		case 12: case -12: case 14: case -14: case 16: case -16:
			if(!p_quiet){G4cout << "Particle: neutrino" << G4endl;} break;

		case 2112: case 2212:
			if(!p_quiet){G4cout << "Particle: neutron/proton" << G4endl;} break;

		// Mesons metastable in Geant4
		case 321: case -321:
			if(!p_quiet){G4cout << "Particle: K +/- meson" << G4endl;} break;
		case 211: case -211:
			if(!p_quiet){G4cout << "Particle: pi +/- meson" << G4endl;} break;
		case 130:
			if(!p_quiet){G4cout << "Particle: K_0L meson" << G4endl;} break;

		default:
			G4cout << "Unknown channel: " << channel << G4endl;
			exit(1);
			break;
	}

	if(pythia_only && (p_cre!=CRE_PYTHIA8 && p_cre!=CRE_PYTHIA8SINGLE)) {
		G4cout << "ERROR: Unable to use Geant4 for this particle!" << G4endl;
		exit(1);
	}

	PGAInterface* generatorAction = NULL;
	switch(p_cre) {
		case CRE_PYTHIA8:
			if(!p_quiet){G4cout << "Standard particle generation using Pythia8" << G4endl;}
			if(p_kinetic) {
				G4cout << "ERROR: can't specify energy as kinetic for this generator!" << G4endl;
				exit(1);
			}
			generatorAction = new DMPythiaPGA(channel, dm_mass, seedvalue);
			break;

		case CRE_PYTHIA8SINGLE:
			if(!p_quiet){G4cout << "Standard particle generation using Pythia8 (single particle)" << G4endl;}
			if(p_kinetic) {
				G4cout << "ERROR: kinetic not implemented for this generator!" << G4endl;
				exit(1);
			}
			generatorAction = new DMPythiaPGA(channel, dm_mass, seedvalue, true);
			break;

		case CRE_GEANT4:
			if(!p_quiet){G4cout << "Geant4 particle generation (particle+antiparticle)" << G4endl;}
			if(p_kinetic) {
				G4cout << "ERROR: can't specify energy as kinetic for this generator!" << G4endl;
				exit(1);
			}
			generatorAction = new Geant4PGA(channel, -channel, dm_mass);
			break;

		case CRE_GEANT4SINGLE:
		if(!p_quiet){G4cout << "Geant4 particle generation (single particle)" << G4endl;}
			generatorAction = new Geant4SinglePGA(channel, dm_mass, p_kinetic);
			break;

		default:
			G4cout << "ERROR: bad creator (p_cre) value" << G4endl;
			exit(1);
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
	G4err << "No visualization compiled!" << G4endl;
#endif
}
