#include "DMPythiaPGA.hh"

int main(int argc, char* argv[]) {
	G4cout << "DMPythiaPGA test" << G4endl;
	
	Pythia8Interface p(25, 1.*TeV);
	for(int i=0; i<3; i++) {
		p.next();
	}
	p.statistics();
	
	return 0;
}
