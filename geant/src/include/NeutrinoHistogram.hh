#ifndef NeutrinoHistogram_h
#define NeutrinoHistogram_h

class G4Track;
class G4ParticleDefinition;
class G4RootAnalysisManager;

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
	
	private:
		G4RootAnalysisManager * anm;
		G4int h;
		
	// Static helpers
	public:
		static nu getNeutrinoType(const G4ParticleDefinition* g4def);
};

#endif
