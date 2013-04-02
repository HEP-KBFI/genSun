#include "SunDetectorConstruction.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

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
	// TODO: Implement the real sun material!

	// Define materials via NIST manager
	G4NistManager* nistManager = G4NistManager::Instance();
	G4bool fromIsotopes = false;
	G4Material* material = nistManager->FindOrBuildMaterial(fMaterialName, fromIsotopes);
	return material;
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
