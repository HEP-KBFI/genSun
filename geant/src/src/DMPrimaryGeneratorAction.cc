#include "DMPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"

DMPrimaryGeneratorAction::DMPrimaryGeneratorAction(      
	const G4String& particleName, 
	G4double energy,
	G4ThreeVector position, 
	G4ThreeVector momentumDirection
) : G4VUserPrimaryGeneratorAction(), fParticleGun(0) {
	G4int nofParticles = 1;
	fParticleGun  = new G4ParticleGun(nofParticles);

	// default particle kinematic
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
	fParticleGun->SetParticleDefinition(particle);
	fParticleGun->SetParticleEnergy(energy);
	fParticleGun->SetParticlePosition(position);
	fParticleGun->SetParticleMomentumDirection(momentumDirection);
}

DMPrimaryGeneratorAction::~DMPrimaryGeneratorAction() {
  delete fParticleGun;
}

void DMPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// this function is called at the begining of event
	fParticleGun->GeneratePrimaryVertex(anEvent);
}

