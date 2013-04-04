#include "DMPythiaPGA.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

#include "Pythia.h"

DMPythiaPGA::DMPythiaPGA(      
	//const G4String& channel, 
	int channel,
	G4double dm_mass,
	G4ThreeVector position, 
	G4ThreeVector momentumDirection
) : G4VUserPrimaryGeneratorAction(), fParticleGun(0), pythia(channel, dm_mass) {
	G4int nofParticles = 1;
	fParticleGun  = new G4ParticleGun(nofParticles);

	// default particle kinematic
	
	G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("mu-");

	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleEnergy(dm_mass);
	fParticleGun->SetParticlePosition(position);
	fParticleGun->SetParticleMomentumDirection(momentumDirection);
}

DMPythiaPGA::~DMPythiaPGA() {
	delete fParticleGun;
}

void DMPythiaPGA::GeneratePrimaries(G4Event* anEvent) {
	// this function is called at the begining of event
	fParticleGun->GeneratePrimaryVertex(anEvent);
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
	G4double dm_mass
) {
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
	
	//pythia->init(25, 25, 1000/GeV);
	pythia->init();
}

Pythia8Interface::~Pythia8Interface() {
	delete pythia;
}

void Pythia8Interface::next() {
	Pythia8::Event ev = pythia->next();
	
	G4cout << "next() called - " << ev.size() << " events." << G4endl;
	
	for(int i=0; i<ev.size(); i++) {
		G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(ev[i].id());
		G4cout << particle->GetParticleName() << G4endl;
	}
}
