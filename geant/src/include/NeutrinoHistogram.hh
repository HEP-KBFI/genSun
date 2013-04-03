#ifndef NeutrinoHistogram_h
#define NeutrinoHistogram_h

class G4Track;
class G4ParticleDefinition;

enum nu {
	nuE, nuMu, nuTau,
	nuEBar, nuMuBar, nuTauBar,
	nuNot
};

class NeutrinoHistogram {
	public:
		NeutrinoHistogram();
		~NeutrinoHistogram();
		
		void addParticle(const G4Track* tr);
		
	// Static helpers
	public:
		static nu getNeutrinoType(const G4ParticleDefinition* g4def);
};

#endif
