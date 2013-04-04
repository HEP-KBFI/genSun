#include "DMPrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"

DMPrimaryGeneratorAction::DMPrimaryGeneratorAction(      
	G4int p1id, G4int p2id,
	G4double dm_mass,
	G4ThreeVector position, 
	G4ThreeVector momentumDirection
) : G4VUserPrimaryGeneratorAction() {
	G4int nofParticles = 1;
	fPGun1 = new G4ParticleGun(nofParticles);
	fPGun2 = new G4ParticleGun(nofParticles);

	// default particle kinematic
	fPGun1->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle(p1id) );
	fPGun1->SetParticleEnergy(dm_mass);
	fPGun1->SetParticlePosition(position);
	fPGun1->SetParticleMomentumDirection(momentumDirection);
	
	fPGun2->SetParticleDefinition( G4ParticleTable::GetParticleTable()->FindParticle(p2id) );
	fPGun2->SetParticleEnergy(dm_mass);
	fPGun2->SetParticlePosition(position);
	fPGun2->SetParticleMomentumDirection(-momentumDirection);
}

DMPrimaryGeneratorAction::~DMPrimaryGeneratorAction() {
  delete fPGun1;
  delete fPGun2;
}

void DMPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	// this function is called at the begining of event
	fPGun1->GeneratePrimaryVertex(anEvent);
	fPGun2->GeneratePrimaryVertex(anEvent);
}
