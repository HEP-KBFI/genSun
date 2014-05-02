#include "DMPythiaPGA.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "Pythia.h"

extern bool p_quiet;

// ---------------------------------------------------------------------
//                        class DMPythiaPGA
// ---------------------------------------------------------------------

DMPythiaPGA::DMPythiaPGA(      
	int channel,
	G4double dm_mass,
	int seedvalue,
	bool single,
	G4ThreeVector position, 
	G4ThreeVector momentumDirection
) : PGAInterface(), pythia(channel, dm_mass, single, seedvalue),
    init_position(position), init_momentum(momentumDirection) {}

DMPythiaPGA::~DMPythiaPGA() {}

void DMPythiaPGA::GeneratePrimaries(G4Event* anEvent) {
	PhysicalParticleList ps = pythia.generate();
	for(PhysicalParticleList::iterator it = ps.begin(); it != ps.end(); it++) {
		PhysicalParticle p = *it;
		
		G4ParticleGun gun(p.pdef, 1); //pointer a to G4 service class
		gun.SetParticleEnergy(p.E);
		gun.SetParticlePosition(init_position);
		gun.SetParticleMomentumDirection(init_momentum);
		gun.GeneratePrimaryVertex(anEvent);
	}
}

// ---------------------------------------------------------------------
//                     PYTHIA 8 related stuff
// ---------------------------------------------------------------------

// A derived class for (e+ e- ->) GenericResonance -> various final states.
class Sigma1GenRes : public Pythia8::Sigma1Process {
	public:
		Sigma1GenRes() {}
		// Evaluate sigmaHat(sHat): dummy unit cross section.
		virtual double sigmaHat() {return 1.;}
		// Select flavour. No colour or anticolour.
		virtual void setIdColAcol() {
			setId( -11, 11, 999999);
			setColAcol( 0, 0, 0, 0, 0, 0);
		}
		// Info on the subprocess.
		virtual string name()    const {return "GenericResonance";}
		virtual int    code()    const {return 9001;}
		virtual string inFlux()  const {return "ffbarSame";}
};

Pythia8Interface::Pythia8Interface(
	int pid,
	G4double dm_mass,
	bool single,
	int seed
) : gunparticle(NULL) {
	if(!p_quiet){
		G4cout << "Initializing PYTHIA: " << pid << " at " << dm_mass/GeV << " GeV"
		       << (single ? " (single particle mode)" : "(standard DM resonance)")
		       << G4endl;
	}
	char ch[1000];
	this->pythia = new Pythia8::Pythia();

	if(!single) {
		// The standard mode - decay of a resonance to two particles.
		if(dm_mass < 5*GeV) {
			G4cout << "ERROR: PYTHIA cannot handle eCM < 10 GeV!" << G4endl;
			exit(1);
		}

		pythia->readFile("pythia.card");
		sprintf(ch,"Beams:eCM = %1.2f", 2*dm_mass/GeV); pythia->readString(ch);

		pythia->readString("999999:onMode = off");
		int apid = (pid==23||pid==25) ? pid : -pid;
		if (pid != 1) sprintf(ch,"999999:onIfAll %d %d", pid, apid); //turn on (id -id)
		else sprintf(ch,"999999:onIfAny 1 2 3"); //Turn on light quarks
		pythia->readString(ch);

		// A class to generate the fictitious resonance initial state.
		Pythia8::SigmaProcess* sigma1GenRes = new Sigma1GenRes();
		pythia->setSigmaPtr( sigma1GenRes);
	} else {
		// The "particle gun" type mode, creating a single particle with energy dm_mass
		pythia->readFile("pythia.card");
		pythia->readString("ProcessLevel:all = off");
		pythia->readString("Standalone:allowResDec = on");

		double pp, mm, ee;
		mm = pythia->particleData.mass(pid)*GeV;
		if(dm_mass < mm) {
			G4cout << "NOTICE: dm_mass below particle mass - assuming particle at rest" << G4endl;
			ee = mm; pp = 0;
		} else {
			ee = dm_mass;
			pp = Pythia8::sqrtpos(ee*ee - mm*mm);
		}

		G4cout << "Adding single particle:" << G4endl;
		G4cout << "  mm = " << mm/MeV << " MeV" << G4endl;
		G4cout << "  pp = " << pp/GeV << " GeV" << G4endl;
		G4cout << "  ee = " << ee/GeV << " GeV" << G4endl;

		gunparticle = new Pythia8::Particle(pid, 2);
		gunparticle->p(pp/GeV, 0.0, 0.0, ee/GeV);
		gunparticle->m(mm/GeV);
	}

	if(!p_quiet){G4cout << "Init PYTHIA seed to: " << seed << G4endl;}
	pythia->readString("Random:setSeed = on");
	sprintf(ch, "Random:seed = %d", seed); pythia->readString(ch);
	if(!p_quiet){G4cout << "PYTHIA conf: " << ch << G4endl;}

	pythia->init();
	pythia->rndm.init(seed);
}

Pythia8Interface::~Pythia8Interface() {
	delete pythia;
}

PhysicalParticleList Pythia8Interface::generate() {
	PhysicalParticleList ps;

	if(gunparticle) {
		pythia->event.reset();
		pythia->event.append(*gunparticle);
	}

	if(!pythia->next()) {
		G4cout << "ERROR: No pythia->next() !" << G4endl;
		return ps;
	}

	Pythia8::Event ev = pythia->event;
	if(!p_quiet){G4cout << "pythia->next() called: " << ev.size() << " events." << G4endl;}
	
	for(int i=0; i<ev.size(); i++) {
		if(!p_quiet) {
			G4cout << " > [" << (ev[i].isFinal()?'x':' ') << "] "
			       << ev[i].name() << "(" << ev[i].id() << "): "
			       << "p(" << ev[i].e() << ", "
			       << ev[i].px() << ", " << ev[i].py() << ", " << ev[i].pz() << "); "
			       << "m(" << ev[i].m() << ")"
			       << G4endl;
		}
		
		if(ev[i].isFinal()) {
			PhysicalParticle p;
			
			p.pdef = G4ParticleTable::GetParticleTable()->FindParticle(ev[i].id());
			p.E = ev[i].e() * GeV;
			p.p = G4ThreeVector(ev[i].px(), ev[i].py(), ev[i].pz()); // TODO: Fix units
			p.r = G4ThreeVector(ev[i].xDec(), ev[i].yDec(), ev[i].zDec()); // TODO: Fix units
			
			if(p.pdef == NULL) {
				G4cerr << "Null pointer in a final particle!" << G4endl;
			}
			
			ps.push_back(p);
		}
	}
	return ps;
}

void Pythia8Interface::statistics() {
	pythia->statistics();
}
