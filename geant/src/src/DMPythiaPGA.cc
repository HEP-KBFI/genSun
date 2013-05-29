#include "DMPythiaPGA.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "Pythia.h"

extern bool p_quiet;

DMPythiaPGA::DMPythiaPGA(      
	//const G4String& channel, 
	int channel,
	G4double dm_mass,
	int seedvalue,
	G4ThreeVector position, 
	G4ThreeVector momentumDirection
) : PGAInterface(), pythia(channel, dm_mass, seedvalue),
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
	int seed
) {
	if(!p_quiet){G4cout << "Initializing PYTHIA: " << pid << " at " << dm_mass/GeV << " GeV" << G4endl;}
	
	char ch[1000];
	
	this->pythia = new Pythia8::Pythia();
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
	
	if(!p_quiet){G4cout << "Init PYTHIA seed to: " << seed << G4endl;}
	pythia->readString("Random:setSeed = on");
	sprintf(ch, "Random:seed = %d", seed); pythia->readString(ch);
	if(!p_quiet){G4cout << "PYTHIA conf: " << ch << G4endl;}
	
	//pythia->init(25, 25, 1000/GeV);
	pythia->init();
	pythia->rndm.init(seed);
}

Pythia8Interface::~Pythia8Interface() {
	delete pythia;
}

PhysicalParticleList Pythia8Interface::generate() {
	if(!pythia->next()) {
		G4cerr << "No next() !" << G4endl;
	}
	Pythia8::Event ev = pythia->event;
	if(!p_quiet){G4cout << "pythia->next() called: " << ev.size() << " events." << G4endl;}
	
	PhysicalParticleList ps;
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
