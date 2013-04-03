#include "SunDetectorConstruction.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4UnitsTable.hh"
#include "G4NistManager.hh"

// Parameters of the sun
G4double density     = 160*g/cm3;
G4double pressure    = 1e9*atmosphere;
G4double temperature = 15e6*kelvin;
ElementFraction sunfractions[] = {
	{"G4_H", 73.46},
	{"G4_He", 24.85}
};

SunDetectorConstruction::SunDetectorConstruction(
	const G4String& materialName,
	G4double hx, G4double hy, G4double hz
) : G4VUserDetectorConstruction(),
	fMaterialName(materialName),
	fDimensions(hx, hy, hz),
	fWorldVolume(0)
{}

SunDetectorConstruction::~SunDetectorConstruction() {}

G4VPhysicalVolume* SunDetectorConstruction::Construct() {
	G4Material* material = this->getSunMaterial();

	// World
	G4Box* sWorld = new G4Box("World", // name
		// dimensions (half-lentghs)
		fDimensions.x(),
		fDimensions.y(), 
		fDimensions.z()
	);

	// Logical World Volume. Arguments: // shape, material, name
	fWorldVolume = new G4LogicalVolume(sWorld, material, "World");

	G4VPhysicalVolume* pWorld = new G4PVPlacement(
		0,                      // no rotation
		G4ThreeVector(),        // at (0,0,0)
		fWorldVolume,           // logical volume
		"World",                // name
		0,                      // no mother volume
		false,                 // no boolean operation
		0                       // copy number
	);

	return pWorld; // always return the root volume
}

G4Material * SunDetectorConstruction::getSunMaterial() {
	// Define materials via NIST manager
	G4NistManager* nm = G4NistManager::Instance();
	nm->SetVerbose(1);
	/*G4bool fromIsotopes = false;
	G4Material* material = nistManager->FindOrBuildMaterial(fMaterialName, fromIsotopes);
	return material;*/
	
	// Calculate the total fraction. Used for normalization.
	double totalfraction = 0.0;
	for(size_t i=0; i < sizeof(sunfractions)/sizeof(sunfractions[0]); i++) {
		totalfraction += sunfractions[i].fraction;
	}
	G4cout << "Total fraction: " << totalfraction << G4endl;
	
	G4Material* solarmaterial = new G4Material(
		"Sun", density, // name, density
		2, kStateGas, // ncomponents, state
		temperature, pressure // temperature, pressure
	);
	for(size_t i=0; i < sizeof(sunfractions)/sizeof(sunfractions[0]); i++) {
		solarmaterial->AddMaterial(
			nm->FindOrBuildMaterial(sunfractions[i].name),
			sunfractions[i].fraction/totalfraction
		);
	}
	/*#define FOB(s) (nm->FindOrBuildMaterial(s))
	solarmaterial->AddMaterial(FOB("G4_H"), 0.7346); // material, fractionmass
	solarmaterial->AddMaterial(FOB("G4_He"), 0.2485);
	#undef FOB*/
	
	G4cout << "Solar material:" << G4endl;
	G4cout << *solarmaterial << G4endl;
	
	return solarmaterial;
}

/*void SunDetectorConstruction::SetMaterial(const G4String& materialName) {
  G4NistManager* nistManager = G4NistManager::Instance();
  G4bool fromIsotopes = false;

  G4Material* newMaterial
    = nistManager->FindOrBuildMaterial(materialName, fromIsotopes);

  if ( ! newMaterial ) {
    G4cerr << "Material " << materialName << " not found." << G4endl;
    G4cerr << "The box material was not changed." << G4endl;
    return;
  }  
   
  if ( fWorldVolume ) fWorldVolume->SetMaterial(newMaterial);
  G4cout << "Material of box changed to " << materialName << G4endl;
}
 
void SunDetectorConstruction::SetDimensions(
	G4double hx, G4double hy, G4double hz
) {
	/// Set world dimension (in half lengths).
	/// This setting has effect only if called in PreInit> phase
	fDimensions = G4ThreeVector(hx, hy, hz);
}*/
