#include "DMPythiaPGA.hh"

#include "G4RunManager.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"

int main(int argc, char* argv[]) {
	G4cout << "DMPythiaPGA test" << G4endl;
	
	G4RunManager* runManager = new G4RunManager;
	G4VModularPhysicsList* physlist;
	G4PhysListFactory factory;
	physlist = factory.GetReferencePhysList("QGSP_BERT");
	runManager->SetUserInitialization(physlist); // physics
	
	Pythia8Interface pythia(25, 1.*TeV);
	for(int i=0; i<3; i++) {
		PhysicalParticleList ps = pythia.generate();
		for(PhysicalParticleList::iterator it = ps.begin(); it != ps.end(); it++) {
			PhysicalParticle p = *it;
			G4cout << "   " << p.pdef->GetParticleName() << " ptr(" << p.pdef << ")" << G4endl;
			G4cout << "   " << p.E << " " << p.p << G4endl;
		}
	}
	pythia.statistics();
	
	return 0;
}
