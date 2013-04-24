#include "SunDetectorConstruction.hh"

#include "globals.hh"
#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4UnitsTable.hh"
#include "G4NistManager.hh"

extern bool p_quiet;

// Parameters of the sun
G4double sun_density     = 160*g/cm3;
G4double sun_pressure    = 1e9*atmosphere;
G4double sun_temperature = 15e6*kelvin;
ElementFraction sun_fractions[] = {
	{"G4_H", 73.46},
	{"G4_He", 24.85}
};

SunDetectorConstruction::SunDetectorConstruction(G4double radius, bool vacuum)
	: G4VUserDetectorConstruction(),fRadius(radius),useVacuum(vacuum) {}

SunDetectorConstruction::~SunDetectorConstruction() {}

G4VPhysicalVolume* SunDetectorConstruction::Construct() {
	G4Material* material = useVacuum ? this->getVacuumMaterial() : this->getSunMaterial();

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
	size_t Nfractions = sizeof(sun_fractions)/sizeof(sun_fractions[0]);
	double totalfraction = 0.0;
	for(size_t i=0; i < Nfractions; i++) {
		totalfraction += sun_fractions[i].fraction;
	}
	
	G4Material* solarmaterial = new G4Material(
		"Sun", sun_density, // name, density
		2, kStateGas, // ncomponents, state
		sun_temperature, sun_pressure // temperature, pressure
	);
	for(size_t i=0; i < Nfractions; i++) {
		solarmaterial->AddMaterial(
			nm->FindOrBuildMaterial(sun_fractions[i].name),
			sun_fractions[i].fraction/totalfraction
		);
	}
	
	if(!p_quiet) {
		G4cout << "==================  Solar material  ==================" << G4endl;
		G4cout << "Total fraction: " << totalfraction << G4endl;
		G4cout << *solarmaterial << G4endl;
	}
	
	return solarmaterial;
}

G4Material * SunDetectorConstruction::getVacuumMaterial() {
	G4double vac_density     = universe_mean_density; //from PhysicalConstants.h
	G4double vac_pressure    = 1.e-19*pascal;
	G4double vac_temperature = 0.1*kelvin;
	return new G4Material("Vacuum", 1., 1.01*g/mole,
	                       vac_density, kStateGas, vac_temperature,
	                       vac_pressure);
}
