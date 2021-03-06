#ifndef DMRootHistogrammer_h
#define DMRootHistogrammer_h

#include <vector>
#include "globals.hh"

#include "Nugrammer.hh"

class G4Track;
class TH1F;

struct HistParams {
	char title[50];
	int nbins;
	double xmin, xmax;
};
const HistParams energyhist_default = {"Energy spectrum", 300, -9, 0};
const HistParams statushist_default = {"Event status", 2, -0.5, 1.5};

class DMRootHistogrammer {
	public:
		DMRootHistogrammer(
			G4int channel_id, G4double dm_mass, const char * physics,
			HistParams energyhist = energyhist_default,
			HistParams statushist = statushist_default
		);
		~DMRootHistogrammer();
		
		void addParticle(const G4Track* track);
		void countEvent(const G4int n=1);
		
		void save(const char* fname);
	
	private:
		Nugrammer ngr;
		TH1F * h_evstatus, * h_pcounter;
		
		G4int channel;
		G4double dm_mass;
		const char * physics;
};

#endif
