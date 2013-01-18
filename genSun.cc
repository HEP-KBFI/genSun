// Copyright (C) 2009-2010 Torbjorn Sjostrand, Mario Kadastik
// This code is based on the main29.cc code from Pythia8 distribution

// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

#include "Pythia.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

using namespace Pythia8;

class BDecay : public DecayHandler {
public:
    
    BDecay(ParticleData* pdtPtrIn, Rndm* rndmPtrIn) {
        pdtPtr = pdtPtrIn; rndmPtr = rndmPtrIn;
    }
    
    bool decay(vector<int>& idProd, vector<double>& mProd,
               vector<Vec4>& pProd, int iDec, const Event& event);
    
private:
    
    
    // Pointer to the particle data table.
    ParticleData* pdtPtr;
    
    // Pointer to the random number generator.
    Rndm* rndmPtr;
    
};

//--------------------------------------------------------------------------

// The actual J/psi decay routine.
// Not intended for realism, just to illustrate the principles.

bool BDecay::decay(vector<int>& idProd, vector<double>& mProd,
                   vector<Vec4>& pProd, int iDec, const Event& event) {
    
    cout << "Decaying particle " << iDec << ", status: " << event[iDec].status() << "\n";
    if (event[iDec].statusAbs() == 93 || event[iDec].statusAbs() == 94) {
        cout << "This particle has already been decayed by energy reduction, decaying normally.\n";
        return false;
    }
    int id = idProd[0];
    double m = mProd[0];
    Vec4 p4 = pProd[0];
    idProd.push_back(id);
    mProd.push_back(m);
   
    //FIXME: Put proper energy reduction formula here
    double e_new = 0.8*p4.e();
    
    Vec4 p4_out(p4.px(), p4.py(), p4.pz(), e_new);
    pProd.push_back(p4_out);
    
    idProd.push_back(39);
    mProd.push_back(0);
    pProd.push_back(p4-p4_out);
    
    //This particle decayed successfully externally
    return true;
    
}

//**************************************************************************

// A derived class for (e+ e- ->) GenericResonance -> various final states.

class Sigma1GenRes : public Sigma1Process {
    
public:
    
    // Constructor.
    Sigma1GenRes() {}
    
    // Evaluate sigmaHat(sHat): dummy unit cross section.
    virtual double sigmaHat() {return 1.;}
    
    // Select flavour. No colour or anticolour.
    virtual void setIdColAcol() {setId( -11, 11, 999999);
        setColAcol( 0, 0, 0, 0, 0, 0);}
    
    // Info on the subprocess.
    virtual string name()    const {return "GenericResonance";}
    virtual int    code()    const {return 9001;}
    virtual string inFlux()  const {return "ffbarSame";}
    
};

//**************************************************************************
int main(int argc, char **argv) {
    if (argc < 5) {
        cout << "Usage: ./gen part DMmass output.root params.card" << endl;
        return 1;
    }
    int partId, apartId;
    double dmMass;
    partId = atoi(argv[1]);
    dmMass = atof(argv[2]);
    if (partId == 23 || partId == 25) apartId = partId; else apartId = -partId;
    // Pythia generator.
    Pythia pythia;
    
    // A class to generate the fictitious resonance initial state.
    SigmaProcess* sigma1GenRes = new Sigma1GenRes();
    
    // Hand pointer to Pythia.
    pythia.setSigmaPtr( sigma1GenRes);
    
    cout << "argv:(" << argv[4] << ")\n";
    
    cout << "test";
    // Read in the rest of the settings and data from a separate file.
    pythia.readFile(argv[4]);
    
    // Select the decay channel
    char ch[60];
    
    if (partId != 1)
        sprintf(ch,"999999:onIfAll %d %d", partId, apartId);
    else
        sprintf(ch,"999999:onIfAny 1 2 3");
    
    pythia.readString("999999:onMode = off");
    pythia.readString(ch);
    
    // Set beam CM energy to 2*DMmass for annihilation. This is equivalent to decay of twice as massive DM particle
    sprintf(ch,"Beams:eCM = %1.2f",2*dmMass);
    pythia.readString(ch);
    // Initialization.
    
    // Extract settings to be used in the main program.
    int    nEvent  = pythia.mode("Main:numberOfEvents");
    int    nList   = pythia.mode("Main:numberToList");
    int    nShow   = pythia.mode("Main:timesToShow");
    int    nAbort  = pythia.mode("Main:timesAllowErrors");
    bool   showCS  = pythia.flag("Main:showChangedSettings");
    bool   showCPD = pythia.flag("Main:showChangedParticleData");
    
    
    DecayHandler* handleDecays = new BDecay(&pythia.particleData,
                                            &pythia.rndm);
    // The list of particles the class can handle.
    vector<int> handledParticles;
    handledParticles.push_back(4);
    handledParticles.push_back(5);
    //handledParticles.push_back(13);
    pythia.setDecayPtr( handleDecays, handledParticles);    

    pythia.init();
    cout << "Generating " << nEvent << " events of DM with mass " << dmMass << " GeV annihilating to " << partId << endl;
    // List changes.
    if (showCS)  pythia.settings.listChanged();
    if (showCPD) pythia.particleData.listChanged();
    
//    // Open the root file for output
    TFile f(argv[3],"RECREATE");
//    
//    // Initialize the histograms
    TH1D *hantip = new TH1D("antip","Antiproton distribution",300,-9,0);
    TH1D *hantin = new TH1D("antin","Antineutron distribution",300,-9,0);
    TH2D *hantid = new TH2D("antid","Antideteron distribution",300,-9,0,400,0,0.4);
    TH1D *hel = new TH1D("el","Electron distribution",300,-9,0);
    TH1D *hnuel = new TH1D("nuel","Electron nu distribution",300,-9,0);
    TH1D *hnumu = new TH1D("numu","Muon nu distribution",300,-9,0);
    TH1D *hnutau = new TH1D("nutau","Tau nu distribution",300,-9,0);
    TH1D *hgam = new TH1D("gam","Gamma distribution",300,-9,0);
    vector<Particle> antip;
    vector<Particle> antin;
    // Begin event loop.
    int iAbort = 0;
    for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
//        if (iEvent%(max(1,nEvent/nShow)) == 0) cout << " Now begin event "
//            << iEvent << "\n";
        
        // Generate events. Quit if many failures.
        if (!pythia.next()) {
            if (++iAbort < nAbort) continue;
            cout << " Event generation aborted prematurely, owing to error!\n";
            break;
        }
        
        // Here's where we have to decay the particles that were still left intact
        // 1. loop over all particles and add at the end of the event copies of those while they are standing still
        int nn = pythia.event.size();
        //cout << "Event originally had " << pythia.event.size() << " particles" << endl;
        for (int i=0; i < nn; ++i)
            if (pythia.event[i].isFinal()) {
                int id = pythia.event[i].id();
                int idAbs = abs(id);
                double m = pythia.event[i].mass();
                if (idAbs == 13 || idAbs == 15 || idAbs == 4 || idAbs == 5 || idAbs == 6 || idAbs == 23 || idAbs == 24 || idAbs == 25) {
                    // If it's one of the relevant particles, then add a respective particle to the event, but at rest
                    pythia.event.append(id,1,0,0,0.,0.,0.,m,m);
                    //cout << " New particle: " << ni << " mayDecay = " << pythia.event[ni].mayDecay() << " tau=" << pythia.event[ni].tau() << endl;
                }
            }
        
        //cout << "With my additions it's " << pythia.event.size() << " particles" << endl;
        // Force the new particles to decay
        //pythia.readString("ParticleDecays:limitTau = off");
        pythia.moreDecays();
        //pythia.readString("ParticleDecays:limitTau = on");
        //cout << "And after decays it's  " << pythia.event.size() << " particles" << endl;
        
        // List first few events.
        if (iEvent < nList) {
            pythia.info.list();
            pythia.event.list();
        }
        
        // Loop over all particles and analyze the final-state ones.
        for (int i = 0; i < pythia.event.size(); ++i)
            if (pythia.event[i].isFinal()) {
                int id = pythia.event[i].id();
                int idAbs = abs(id);
                double x = log10((pythia.event[i].e()-pythia.event[i].m())/dmMass);
                if (id == -2212) {
                    antip.push_back(pythia.event[i]);
                    hantip->Fill(x);
                }
                if (id == -2112) {
                    antin.push_back(pythia.event[i]);
                    hantin->Fill(x);
                }
            }
        pythia.particleData.mayDecay(2112,true);
        pythia.moreDecays();
        pythia.particleData.mayDecay(2112,false);
        for (int i = 0; i < pythia.event.size(); ++i)
            if (pythia.event[i].isFinal()) {
                int id = pythia.event[i].id();
                int idAbs = abs(id);
                double x = log10((pythia.event[i].e()-pythia.event[i].m())/dmMass);
                if (idAbs == 11) hel->Fill(x);
                if (idAbs == 12) hnuel->Fill(x);
                if (idAbs == 14) hnumu->Fill(x);
                if (idAbs == 16) hnutau->Fill(x);
                if (id == 22) hgam->Fill(x);
                if (id == -2212) hantip->Fill(x);
            }
        // Loop over anti-partons and fill histograms
        for (vector<Particle>::iterator ap = antip.begin(); ap != antip.end(); ap++)
            for (vector<Particle>::iterator an = antin.begin(); an != antin.end(); an++) {
                Vec4 d = ap->p() - an->p();
                Vec4 d2 = ap->p() + an->p();
                double y = sqrt(pow(d.px(),2)+pow(d.py(),2)+pow(d.pz(),2)-pow(ap->e()-an->e(),2));
                double x = log10((d2.e()-ap->m()-an->m())/dmMass);
                if (y < 0.16 && ap->status() > 0 && an->status() > 0) {
                    hantid->Fill(x,y);
                    ap->statusNeg();
                    an->statusNeg();
                }
            }
        
        // clean up
        antip.clear();
        antin.clear();
        // End of event loop.
    }
    cout << "Writing output" << endl;
    f.cd();
    f.Write();
    f.Close();
    // Done.
    return 0;
}
