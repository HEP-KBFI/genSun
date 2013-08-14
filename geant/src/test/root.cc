#include <iostream>

#include "TRandom.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"

using namespace std;

bool p_quiet = false;

int main() {
	TRandom *rnd = new TRandom(time(0));
	
	TH1F h1("unit", "", 20, 0, 1);
	TH1F h2("real", "", 50, -5, 5);
	//h.SetDirectory(&dir);
	for(int i=0; i<10000; i++) {
		h1.Fill(rnd->Rndm());
		h2.Fill(rnd->Rndm()*2 - 0.5);
	}
	
	TFile* tfile = new TFile("ofile.root", "RECREATE");
	TDirectory* dir = tfile->mkdir("dir1")->mkdir("dir2");
	dir->cd();
	
	h1.Write();
	h2.Write();
	
	//tfile->cd();
	tfile->ls();
	tfile->Close();
	
	return 0;
}
