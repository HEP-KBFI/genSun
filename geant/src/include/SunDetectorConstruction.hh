#ifndef SunDetectorConstruction_h
#define SunDetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/SystemOfUnits.h"

class G4LogicalVolume;
class G4Material;

class SunDetectorConstruction : public G4VUserDetectorConstruction {
	public:
		// static methods to create materials
		static G4Material * getGeneralSunMaterial(unsigned int Nfractions, G4double temperature);
		static G4Material * getSunMaterial(unsigned int Nfractions=2);
		static G4Material * getIroncoreMaterial();
		static G4Material * getVacuumMaterial();

		// fractions == 0 implies vacuum
		SunDetectorConstruction(G4double radius = 1*m, G4Material * material = SunDetectorConstruction::getSunMaterial());
		~SunDetectorConstruction();

		// methods from base class 
		virtual G4VPhysicalVolume* Construct();
	private:
		G4double fRadius;
		G4LogicalVolume* fWorldVolume;
		G4Material * material;
};

struct ElementFraction {
	G4String name;
	G4double fraction;
};
#endif

