#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include "DMEventWriter.hh"

extern bool p_quiet;

DMEventWriter::DMEventWriter(const char * ofname) {
	if(p_quiet) {std::cout << "Creating ROOT file: " << ofname << std::endl;}
	this->tfile = new TFile(ofname, "RECREATE");
	
	this->tree = new TTree("events", "events");
	TBranch * br0 = this->addBranch("event_id", "I", &f_eventID);
	TBranch * br1 = this->addBranch("particle_id", "I", &f_pid);
	TBranch * br2 = this->addBranch("particle_energy", "D", &f_pE);
	TBranch * br3 = this->addBranch("parent_id", "I", &f_parentid);
	
	//TBranch* br4 = tree->Branch("parent_energy", &f_parentE, "parent_energy/D");
	//TBranch* br5 = tree->Branch("creator", "std::string", &f_creator);
}

DMEventWriter::~DMEventWriter() {
	this->tfile->WriteTObject(this->tree);
	this->tfile->Close();
}

void DMEventWriter::fill(int evid, int pid, double pE, int parentid) {
	std::cout << "DMEventWriter::fill("
	          << "evid=" << evid
	          << ", pid=" << pid
	          << ", pE=" << pE
	          << ", parentid=" << parentid
	          << ")" << std::endl;
	
	this->f_eventID = evid;
	this->f_pid = pid;
	this->f_pE = pE;
	this->f_parentid = parentid;
	
	this->tree->Fill();
}

TBranch * DMEventWriter::addBranch(std::string name, std::string type, void * obj) {
	TBranch * b;
	if(type == "I" || type == "F" ||  type == "D") {
		std::string name_type = name + "/" + type;
		if(!p_quiet){std::cout << "Creating branch `" << name << "` (name_type:" << name_type << ")" << std::endl;}
		b = this->tree->Branch(name.c_str(), obj, name_type.c_str());
	} else {
		if(!p_quiet){std::cout << "Creating branch `" << name << "` (" << type << ")" << std::endl;}
		b = this->tree->Branch(name.c_str(), type.c_str(), obj);
	}
	
	return b;
}
