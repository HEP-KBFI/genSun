#ifndef DMPythiaPGA_h
#define DMPythiaPGA_h

#include <vector>

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4Event;
class G4ParticleDefinition;
namespace Pythia8 { class Pythia; }

struct PhysicalParticle {
	G4ParticleDefinition* pdef;
	G4double E; // energy
	G4ThreeVector r, p; // position, momentum
};
typedef std::vector<PhysicalParticle> PhysicalParticleList;

class Pythia8Interface {
	public:
		Pythia8Interface(int pid, G4double dm_mass);
		~Pythia8Interface();
		
		PhysicalParticleList generate();
		void statistics();
	
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
		Pythia8Interface pythia;
		G4ThreeVector init_position, init_momentum;
};

#endif


