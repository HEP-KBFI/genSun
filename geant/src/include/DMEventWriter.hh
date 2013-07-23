#ifndef DMEventWriter_h
#define DMEventWriter_h

#include <string>

class TFile;
class TTree;
class TBranch;

class DMEventWriter {
	TFile * tfile;
	TTree * tree;
	int f_eventID;
	int f_pid, f_parentid;
	double f_pE;
	
	public:
		DMEventWriter(const char * ofname = "events.root");
		~DMEventWriter();
		
		void fill(int evid, int pid, double pE, int parentid);
		
		TBranch * addBranch(std::string name, std::string type, void * obj);
};

#endif
