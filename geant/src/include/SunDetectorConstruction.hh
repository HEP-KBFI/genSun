#ifndef SunDetectorConstruction_h
#define SunDetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/SystemOfUnits.h"

class G4LogicalVolume;
class G4Material;

class SunDetectorConstruction : public G4VUserDetectorConstruction {
	public:
		// fractions == 0 implies vacuum
		SunDetectorConstruction(G4double radius = 1*m, unsigned int fractions=2);
		~SunDetectorConstruction();

		// methods from base class 
		virtual G4VPhysicalVolume* Construct();

	private:
		G4double fRadius;
		G4LogicalVolume* fWorldVolume;
		unsigned int fractions;

		static G4Material * getSunMaterial(unsigned int Nfractions=2);
		static G4Material * getVacuumMaterial();
};

struct ElementFraction {
	G4String name;
	G4double fraction;
};
#endif

