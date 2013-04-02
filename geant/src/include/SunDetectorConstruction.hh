#ifndef SunDetectorConstruction_h
#define SunDetectorConstruction_h

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "CLHEP/Units/SystemOfUnits.h"

class G4LogicalVolume;
class G4Material;

class SunDetectorConstruction : public G4VUserDetectorConstruction {
	public:
		SunDetectorConstruction(
			const G4String& materialName = "G4_AIR",
			G4double hx = 50*CLHEP::cm, 
			G4double hy = 50*CLHEP::cm, 
			G4double hz = 50*CLHEP::cm
		);
		~SunDetectorConstruction();

		// methods from base class 
		virtual G4VPhysicalVolume* Construct();

		// set methods
		//void  SetMaterial(const G4String& materialName);
		//void  SetDimensions(G4double hx, G4double hy, G4double hz);

	private:
		G4String               fMaterialName;
		G4ThreeVector          fDimensions;
		G4LogicalVolume*       fWorldVolume;  

		G4Material * getSunMaterial();
};

struct ElementFraction {
	G4String name;
	G4double fraction;
};
#endif

