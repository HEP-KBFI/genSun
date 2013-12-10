#include "globals.hh"
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4NeutrinoE.hh"
#include "G4ThreeVector.hh"

#include "DMRootHistogrammer.hh"

bool p_quiet = false;
bool p_trv = true;

int main() {
	DMRootHistogrammer* hg = new DMRootHistogrammer(21,200*MeV,"WHAT");
	hg->countEvent(1); hg->countEvent(6);
	
	G4Track * tr = new G4Track(
		new G4DynamicParticle(
			G4NeutrinoE::NeutrinoEDefinition(),
			G4ThreeVector(1,2,3),
			150*MeV
		),
		123.5235,
		G4ThreeVector(1,2,3)
	);
	hg->addParticle(tr);
	
	hg->save("hehe.root");
	return 0;
}
