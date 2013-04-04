#ifndef DMPrimaryGeneratorAction_h
#define DMPrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

class G4ParticleGun;
class G4Event;

/// The primary generator class with particle gun

class DMPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
	public:
		DMPrimaryGeneratorAction(
			G4int p1id, G4int p2id,
			G4double dm_mass = 1.*CLHEP::MeV,
			G4ThreeVector position= G4ThreeVector(0,0,0),
			G4ThreeVector momentumDirection = G4ThreeVector(0,0,1)
		);    
		~DMPrimaryGeneratorAction();

		// methods
		virtual void GeneratePrimaries(G4Event*);

	private:
		// data members
		G4ParticleGun *fPGun1, *fPGun2; //pointer a to G4 service class
};

#endif


