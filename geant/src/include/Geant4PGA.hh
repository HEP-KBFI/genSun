#ifndef Geant4PGA_h
#define Geant4PGA_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include "PGAInterface.hh"

class G4ParticleGun;
class G4Event;

/// The primary generator class with particle gun

class Geant4PGA : public PGAInterface {
	public:
		Geant4PGA(
			G4int p1id, G4int p2id,
			G4double dm_mass,
			G4ThreeVector position= G4ThreeVector(0,0,0),
			G4ThreeVector momentumDirection = G4ThreeVector(0,0,1)
		);
		~Geant4PGA();

		// methods
		virtual void GeneratePrimaries(G4Event*);
		const char* getName() {return "G4";}

	private:
		// data members
		G4ParticleGun *fPGun1, *fPGun2; //pointer a to G4 service class
};

class Geant4SinglePGA : public PGAInterface {
	public:
		Geant4SinglePGA(
			G4int pid,
			G4double dm_mass,
			G4ThreeVector position= G4ThreeVector(0,0,0),
			G4ThreeVector momentumDirection = G4ThreeVector(0,0,1)
		);
		~Geant4SinglePGA();

		// methods
		virtual void GeneratePrimaries(G4Event*);
		const char* getName() {return "G4single";}

	private:
		// data members
		G4ParticleGun * fPGun; //pointer a to G4 service class
};

#endif


