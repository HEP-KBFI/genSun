#include "Geant4PGA.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"

Geant4PGA::Geant4PGA(
	G4int p1id, G4int p2id,
	G4double dm_mass,
	G4ThreeVector position,
	G4ThreeVector momentumDirection
) : PGAInterface() {
	G4int nofParticles = 1;
	fPGun1 = new G4ParticleGun(nofParticles);
	fPGun2 = new G4ParticleGun(nofParticles);

	double kin_energy_p1 = dm_mass - G4ParticleTable::GetParticleTable()->FindParticle(p1id)->GetPDGMass();
	double kin_energy_p2 = dm_mass - G4ParticleTable::GetParticleTable()->FindParticle(p2id)->GetPDGMass();

	// default particle kinematic
	fPGun1->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle(p1id) );
	fPGun1->SetParticleEnergy(kin_energy_p1);
	fPGun1->SetParticlePosition(position);
	fPGun1->SetParticleMomentumDirection(momentumDirection);

	fPGun2->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle(p2id) );
	fPGun2->SetParticleEnergy(kin_energy_p2);
	fPGun2->SetParticlePosition(position);
	fPGun2->SetParticleMomentumDirection(-momentumDirection);
}

Geant4PGA::~Geant4PGA() {
  delete fPGun1;
  delete fPGun2;
}

void Geant4PGA::GeneratePrimaries(G4Event* anEvent) {
	// this function is called at the begining of event
	fPGun1->GeneratePrimaryVertex(anEvent);
	fPGun2->GeneratePrimaryVertex(anEvent);
}

Geant4SinglePGA::Geant4SinglePGA(
	G4int pid,
	G4double dm_mass,
	G4ThreeVector position,
	G4ThreeVector momentumDirection
) : PGAInterface() {
	G4int nofParticles = 1;
	fPGun = new G4ParticleGun(nofParticles);

	double kin_energy_p1 = dm_mass - G4ParticleTable::GetParticleTable()->FindParticle(pid)->GetPDGMass();

	// default particle kinematic
	fPGun->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle(pid) );
	fPGun->SetParticleEnergy(kin_energy_p1);
	fPGun->SetParticlePosition(position);
	fPGun->SetParticleMomentumDirection(momentumDirection);
}

Geant4SinglePGA::~Geant4SinglePGA() {
  delete fPGun;
}

void Geant4SinglePGA::GeneratePrimaries(G4Event* anEvent) {
	// this function is called at the begining of event
	fPGun->GeneratePrimaryVertex(anEvent);
}
