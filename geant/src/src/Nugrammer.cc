#include "Nugrammer.hh"

#include <TH1F.h>

#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <cmath>

struct particle {
	TH1F* h;
	std::string name;
	std::map<int, TH1F*> ghs;
	particle() {}
	particle(std::string name) : name(name) {}
};

struct Nugrammer::_self {
	unsigned int bins;
	double E0, min, max;
	std::map<int, particle> particles;
	std::map<int, std::string> groups;
	_self(double E0, unsigned int bins, double min, double max)
	 : bins(bins), E0(E0), min(min), max(max) {}
};

Nugrammer::Nugrammer(double E0, unsigned int bins, double min, double max)
 : self(new _self(E0, bins, min, max)) {
	addGroup(0, "other");
}

Nugrammer::~Nugrammer() {delete self;}

void Nugrammer::addParticle(int pid, std::string name) {
	if(self->particles.count(pid) != 0) {
		std::cout << "ERROR: pid already exists! Not adding." << std::endl;
		return;
	}

	particle np(name);
	np.h = new TH1F(NULL, name.c_str(), self->bins, self->min, self->max);

	for(auto it = self->groups.begin(); it != self->groups.end(); ++it) {
		np.ghs[it->first] = new TH1F(NULL, (name + "::" + it->second).c_str(), self->bins, self->min, self->max);
	}

	self->particles[pid] = np;
}

void Nugrammer::addGroup(int gid, std::string name) {
	if(self->groups.count(gid) != 0) {
		std::cout << "ERROR: gid already exists! Not adding." << std::endl;
		return;
	}
	self->groups[gid] = name;

	for(auto it = self->particles.begin(); it != self->particles.end(); ++it) {
		it->second.ghs[gid] = new TH1F(NULL, (it->second.name + "::" + name).c_str(), self->bins, self->min, self->max);
	}
}

void Nugrammer::fill(double E, int pid, int gid) {
	double logE = std::log10(E / self->E0);

	gid = (self->groups.count(gid) == 0) ? 0 : gid;

	try {
		particle& p = self->particles.at(pid);
		p.h->Fill(logE);
		p.ghs.at(gid)->Fill(logE);
	} catch(const std::out_of_range& e) {
		//std::cout << "ERROR: missing pid(" << pid << ") when filling!" << std::endl;
	}
}

void Nugrammer::save(TDirectory* dir, TDirectory* sumdir) {
	sumdir = (sumdir==NULL) ? dir->mkdir("TOTALS") : sumdir;
	for(auto it = self->particles.begin(); it != self->particles.end(); ++it) {
		sumdir->WriteTObject(it->second.h, it->second.name.c_str());
		TDirectory * subdir = dir->mkdir(it->second.name.c_str());
		for(auto it_sub = it->second.ghs.begin(); it_sub != it->second.ghs.end(); ++it_sub) {
			subdir->WriteTObject(it_sub->second, self->groups[it_sub->first].c_str());
		}
	}
}
