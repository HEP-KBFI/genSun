#ifndef NeutrinoHistogram_h
#define NeutrinoHistogram_h

class G4Track;
class G4ParticleDefinition;
class G4RootAnalysisManager;

enum nu_t {
	nuE, nuMu, nuTau,
	nuEBar, nuMuBar, nuTauBar,
	nuNot
};

class NeutrinoHistogram {
	public:
		NeutrinoHistogram(
			G4int pid, G4double dm_mass,
			G4double xmin=0*MeV, G4double xmax=100*MeV,
			G4int nbins=300,
			bool logstyle=true
		);
		~NeutrinoHistogram();
		
		void addParticle(const G4Track* tr);
		void write(G4String ofile);
	
	private:
		G4RootAnalysisManager * anm;
		G4int * hs;
		
	// Static helpers
	public:
		static nu_t getNeutrinoType(const G4ParticleDefinition* g4def);
};

#endif
