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
	{"G4_He", 24.85},
	{"G4_O", 0.77},
	{"G4_C", 0.29},
	{"G4_Fe", 0.16},
	{"G4_Ne", 0.12},
	{"G4_S", 0.10},
	{"G4_N", 0.09},
	{"G4_Si", 0.07},
	{"G4_Mg", 0.05}
};

SunDetectorConstruction::SunDetectorConstruction(G4double radius, G4Material * material)
	: G4VUserDetectorConstruction(),fRadius(radius),material(material) {}

SunDetectorConstruction::~SunDetectorConstruction() {}

G4VPhysicalVolume* SunDetectorConstruction::Construct() {
	if(!p_quiet) {
		G4cout << "==================  Material  ==================" << G4endl;
		G4cout << (*material) << G4endl;
	}

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

G4Material * SunDetectorConstruction::getGeneralSunMaterial(unsigned int Nfractions, G4double temperature) {
	// Define materials via NIST manager
	G4NistManager* nm = G4NistManager::Instance();
	//nm->SetVerbose(1);
	
	// Calculate the total fraction. Used for normalization.
	//size_t Nfractions = sizeof(sun_fractions)/sizeof(sun_fractions[0]);
	double totalfraction = 0.0;
	for(unsigned int i=0; i < Nfractions; i++) {
		totalfraction += sun_fractions[i].fraction;
	}
	
	G4Material* solarmaterial = new G4Material(
		"Sun", sun_density, // name, density
		Nfractions, kStateGas, // ncomponents, state
		temperature, sun_pressure // temperature, pressure
	);
	for(size_t i=0; i < Nfractions; i++) {
		solarmaterial->AddMaterial(
			nm->FindOrBuildMaterial(sun_fractions[i].name),
			sun_fractions[i].fraction/totalfraction
		);
	}
	
	if(!p_quiet) {G4cout << "Sun material - total fraction: " << totalfraction << G4endl;}
	
	return solarmaterial;
}

G4Material * SunDetectorConstruction::getSunMaterial(unsigned int Nfractions) {
	return SunDetectorConstruction::getGeneralSunMaterial(Nfractions, sun_temperature);
}

G4Material * SunDetectorConstruction::getVacuumMaterial() {
	G4double vac_density     = universe_mean_density; //from PhysicalConstants.h
	G4double vac_pressure    = 1.e-19*pascal;
	G4double vac_temperature = 0.1*kelvin;
	return new G4Material("Vacuum", 1., 1.01*g/mole,
	                       vac_density, kStateGas, vac_temperature,
	                       vac_pressure);
}

G4Material * SunDetectorConstruction::getIroncoreMaterial() {
	G4NistManager* nm = G4NistManager::Instance();
	G4Material* solarmaterial = new G4Material(
		"IronCore", sun_density, // name, density
		1, kStateGas, // ncomponents, state
		sun_temperature, sun_pressure // temperature, pressure
	);
	solarmaterial->AddMaterial(nm->FindOrBuildMaterial("G4_Fe"), 1);
	return solarmaterial;
}
