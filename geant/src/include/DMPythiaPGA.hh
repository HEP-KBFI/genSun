#ifndef DMPrimaryGeneratorAction_h
#define DMPrimaryGeneratorAction_h

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;
class G4ParticleDefinition;
namespace Pythia8 { class Pythia; }

struct PhyiscalParticle {
	G4ParticleDefinition* particle;
	G4double energy;
	G4ThreeVector position, momentumDirection;
};

class Pythia8Interface {
	public:
		Pythia8Interface(int pid, G4double dm_mass);
		~Pythia8Interface();
		
		// testing
		void next();
	
	private:
		Pythia8::Pythia* pythia;
};

class DMPythiaPGA : public G4VUserPrimaryGeneratorAction {
	public:
		DMPythiaPGA(
			int channel=25,
			G4double dm_mass = 1.*TeV,
			G4ThreeVector position= G4ThreeVector(0,0,0),
			G4ThreeVector momentumDirection = G4ThreeVector(0,0,1)
		);
		~DMPythiaPGA();

		// methods
		void GeneratePrimaries(G4Event*);

	private:
		// data members
		G4ParticleGun*  fParticleGun; //pointer a to G4 service class
		Pythia8Interface pythia;
};

#endif


