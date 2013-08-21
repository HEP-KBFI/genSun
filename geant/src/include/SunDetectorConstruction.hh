#ifndef SunDetectorConstruction_h
#define SunDetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/SystemOfUnits.h"

class G4LogicalVolume;
class G4Material;

class SunDetectorConstruction : public G4VUserDetectorConstruction {
	public:
		SunDetectorConstruction(G4double radius = 1*m, bool vacuum=false);
		~SunDetectorConstruction();

		// methods from base class 
		virtual G4VPhysicalVolume* Construct();

	private:
		G4double fRadius;
		G4LogicalVolume* fWorldVolume;
		bool useVacuum;

		static G4Material * getSunMaterial(unsigned int Nfractions=2);
		static G4Material * getVacuumMaterial();
};

struct ElementFraction {
	G4String name;
	G4double fraction;
};
#endif

