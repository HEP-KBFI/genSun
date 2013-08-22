#ifndef DMPythiaPGA_h
#define DMPythiaPGA_h

#include <vector>

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

#include "PGAInterface.hh"

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
		Pythia8Interface(int pid, G4double dm_mass, int seed=0);
		~Pythia8Interface();
		
		PhysicalParticleList generate();
		void statistics();
		
		Pythia8::Pythia * getPythia() {return this->pythia;}
	
	private:
		Pythia8::Pythia* pythia;
};

class DMPythiaPGA : public PGAInterface {
	public:
		DMPythiaPGA(
			int channel=25,
			G4double dm_mass = 1.*TeV,
			int seedvalue = 0,
			G4ThreeVector position= G4ThreeVector(0,0,0),
			G4ThreeVector momentumDirection = G4ThreeVector(0,0,1)
		);
		~DMPythiaPGA();

		// methods
		void GeneratePrimaries(G4Event*);
		const char* getName() {return "P8";}

	private:
		// data members
		Pythia8Interface pythia;
		G4ThreeVector init_position, init_momentum;
};

#endif


