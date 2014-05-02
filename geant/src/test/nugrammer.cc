#include "Nugrammer.hh"

#include <TFile.h>

#include <iostream>
using namespace std;

int main(int argc, char * argv[]) {
	cout << "Nugrammer test.." << endl;

	Nugrammer ngr(100e3);
	ngr.addGroup(90, "g1");
	ngr.addParticle(44, "numu");
	ngr.addParticle(45, "anutau");
	ngr.addParticle(45, "anutau");
	ngr.addGroup(90, "g1");
	ngr.addGroup(91, "g2");
	ngr.addParticle(42, "nuel");
	ngr.addGroup(92, "g3");

	ngr.fill(10e3, 44, 90);
	ngr.fill(10e3, 44, 99);
	ngr.fill(10e3, 199, 90);

	TFile tfile("test_nugrammer.root", "RECREATE");
	TDirectory * subdir = tfile.mkdir("a_sub_directory");
	ngr.save(subdir);
	tfile.Close();
}
