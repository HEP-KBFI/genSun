#ifndef Nugrammer_h
#define Nugrammer_h

#include <TDirectory.h>
#include <string>

class Nugrammer {
	struct _self; _self * self;
	Nugrammer(const Nugrammer&) {}
	Nugrammer& operator=(const Nugrammer&) {return *this;}

	public:
	Nugrammer(double E0, unsigned int bins=300, double min=-9, double max=0);
	~Nugrammer();

	void addParticle(int pid, std::string name);
	void addGroup(int gid, std::string name);

	void fill(double E, int pid, int gid);

	void save(TDirectory* dir, TDirectory* sumdir=NULL);
};

#endif
