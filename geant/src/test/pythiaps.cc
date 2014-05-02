#include "DMPythiaPGA.hh"

#include "G4RunManager.hh"
#include "G4VModularPhysicsList.hh"
#include "G4PhysListFactory.hh"

#include "Pythia.h"

#include <boost/iostreams/tee.hpp>

bool p_quiet = false;

int main(int argc, char* argv[]) {
	G4RunManager* runManager = new G4RunManager;
	G4VModularPhysicsList* physlist;
	G4PhysListFactory factory;
	physlist = factory.GetReferencePhysList("QGSP_BERT");
	runManager->SetUserInitialization(physlist); // physics

	Pythia8Interface pythia8interface(25, 1.*TeV);
	Pythia8::Pythia * pythia = pythia8interface.getPythia();

	Pythia8::ParticleData& pdata = pythia->particleData;

	std::cout << "Writing the particle table to `ptable.txt`." << std::endl;
	std::ofstream fout_ptable("ptable.txt");
	pdata.listAll(fout_ptable);
	fout_ptable.close();

	std::cout << "Writing stable particles to `maydecay.txt`." << std::endl;
	int pcount=0, mdcount=0;
	std::ofstream fout_maydecay("maydecay.txt");
	//std::ostream& osmd = std::cout;
	std::ostream& osmd = fout_maydecay;
	osmd << "  PDGID           name  canDecay  mayDecay  colType";
	osmd << " lepton  quark  gluon diquark parton hadron  meson baryon";
	osmd << std::endl;
	for(int id=1; id != 0; id=pdata.nextId(id)) {
		pcount++;
		bool decays = pdata.mayDecay(id) && pdata.canDecay(id);
		bool isMeaningful = pdata.isLepton(id) || pdata.isHadron(id);
		if(!isMeaningful || decays) continue;
		mdcount++;

		osmd << std::setw(7)  << id;
		osmd << std::setw(15) << pdata.name(id).c_str();
		osmd << std::setw(10) << (pdata.canDecay(id)?"Yes":"No");
		osmd << std::setw(10) << (pdata.mayDecay(id)?"Yes":"No");

		osmd << std::setw(9) << pdata.colType(id);

		osmd << std::setw(7)  << (pdata.isLepton(id)?"Yes":"No");
		osmd << std::setw(7)  << (pdata.isQuark(id)?"Yes":"No");
		osmd << std::setw(7)  << (pdata.isGluon(id)?"Yes":"No");
		osmd << std::setw(8)  << (pdata.isDiquark(id)?"Yes":"No");
		osmd << std::setw(7)  << (pdata.isParton(id)?"Yes":"No");
		osmd << std::setw(7)  << (pdata.isHadron(id)?"Yes":"No");
		osmd << std::setw(7)  << (pdata.isMeson(id)?"Yes":"No");
		osmd << std::setw(7)  << (pdata.isBaryon(id)?"Yes":"No");

		osmd << std::endl;
	}
	fout_maydecay.close();
	std::cout << "Particles: " << pcount << std::endl;
	std::cout << "mayDecays: " << mdcount << std::endl;

	//pdata.list(90);

	return 0;
}
