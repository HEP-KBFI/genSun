#include "SunDetectorConstruction.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
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

SunDetectorConstruction::SunDetectorConstruction(G4double radius)
	: G4VUserDetectorConstruction(),fRadius(radius) {}

SunDetectorConstruction::~SunDetectorConstruction() {}

G4VPhysicalVolume* SunDetectorConstruction::Construct() {
	G4Material* material = this->getSunMaterial();

	// World
	G4CSGSolid* sWorld = new G4Orb("World", fRadius);

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
	//nm->SetVerbose(1);
	
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
	}G4cout << "Solar material:" << G4endl;
	G4cout << *solarmaterial << G4endl;
	
	return solarmaterial;
}
