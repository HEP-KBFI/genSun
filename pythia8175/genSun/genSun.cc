// Copyright (C) 2009-2010 Torbjorn Sjostrand, Mario Kadastik
// This code is based on the main29.cc code from Pythia8 distribution

// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.


//#include <cstdlib>
//#include <cassert>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <vector>
#include <iostream>
#include <cmath>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <Pythia.h>
#include "particle.h"
#include "TROOT.h"

Pythia8::Pythia* gPythia=0;


//Enable this to switch on event particle list generation
#define NDEBUG

//Set up the GSL random number generation
const gsl_rng_type * randomGeneratorType = 0;
const gsl_rng * rng = 0;

unsigned int gslErrCount = 0;
unsigned int PythiaErrCount = 0;

void seedRandom();
void seedRandom() {
    ifstream f("/dev/urandom");
    unsigned short seed=0;
    if(f.good()) {
        f.read(reinterpret_cast<char*>(&seed), sizeof(seed));
        cout << "Setting GSL random seed to " << seed << endl;
        f.close();
        
    } else {
        cerr << "Failed to open stream from /dev/urandom." << endl;
    }
    gsl_rng_set(rng, seed);
    return;
}


//Returns the pdgId of the primary quark in the hadron by getting the first digit
// of the absolute value.
int idQuark(int idHad);
int idQuark(int idHad) {
    
    std::ostringstream os;
    os << abs(idHad);
    std::string digits = os.str();
    const char f = digits[0];
    int first = atoi(&f);
    return first;
}

void gslErrorHandler(const char* reason, const char* file, int line, int gsl_errno);
void gslErrorHandler(const char* reason, const char* file, int line, int gsl_errno) {
    gslErrCount++;
    cerr << "GSL error: " << gsl_errno << " in file " << file << " with reason '" << reason << "'\n";
}

//samples a number from the power distribution
double powerDistributionRandom(double xu, double xl, double n) {
    double y = gsl_rng_uniform(rng);
    //    double x = pow(
    //                   (pow(xu, n + 1.0) - pow(xl, n + 1.0))*y + pow(xl, n + 1.0),
    //                   n/(n+1.0)
    //                   );
    double x = pow(y, n);
    
    return x;
}

//Calculates the average energy loss that a B or C hadron suffers in
//dense material. Implemented as in http://arxiv.org/pdf/hep-ph/0506298v5.pdf
//formula 8, 9
namespace avgEnergyLoss {
    double tint = 2.5E-11; // Page 12 of Strumia, Cirelli et al
    //double tdec = 1.0E-12; //Instead take it from the particle table
    
    //quark/hadron mass ratio
    double x(int idQ, int idHad, Pythia8::ParticleData* pdt) {
        if(idQ==4 || idQ==5) { //c or b
            return pdt->m0(idQ)/pdt->m0(idHad);
        }
        return GSL_NAN;
    }
    
    double z(int idQ) {
        if(idQ == 4) { //c
            return 0.6;
        } else if (idQ == 5) { //b
            return 0.8;
        }
        return GSL_NAN;
    }
    
    double Ecr(double E0, int idHad, Pythia8::ParticleData* pdt) {
        double tdec = pdt->tau0(idHad);
        int idQ = idQuark(idHad);
        double Z = x(idQ, idHad, pdt)*z(idQ);
        double tstop = tint/(1.0-Z);
        double Ec = (pdt->m0(idHad))*tstop/tdec;
        return Ec;
    }
    
    double E(double E0, int idHad, Pythia8::ParticleData* pdt) {
        double tdec = pdt->tau0(idHad);
        int idQ = idQuark(idHad);
        double Z = x(idQ, idHad, pdt)*z(idQ);
        double tstop = tint/(1.0-Z);
        double Ec = (pdt->m0(idHad))*tstop/tdec;
        double _x = Ec/E0;
        
        double _E = TMath::QuietNaN();
        if(!std::isnan(_x)) {
            double f = gsl_sf_gamma_inc(0.0, _x); //Integral as incomplete gamma function from gsl
            if (f != GSL_ERANGE) { //no over/underflow
                _E = Ec*exp(_x)*f;
            }
        }
        return _E;
    }
}

namespace energyLossDistributions {
    
    //Charged lepton energy loss rate in the Sun [GeV/s].
    const double chLeptonELossRate = 0.8E10;
    
    // 1 [mm/s] = timeConv [s]
    const double timeConv = 3.33564095E-12;
    
    //Draws the energy loss from the exponential distribution p(x)=\exp(x_0 - x) where
    // x = E_cr / E and x_0 = E_cr / E_0 with E_cr depending on the hadron species
    
    //FIXME: draw value for x from gsl_ran_exponential
    double E_hadronic(double E0, int idHad, Pythia8::ParticleData* pdt) {
        
        // The PDF for x is p(x)=\exp(x_0 - x) with normalization \int_x0^\infty p(x) dx = 1
        // Thus by change of variables u = x_0 - x we have an exponential distribution
        // \exp(u) with mean 1
        double u = gsl_ran_exponential(rng, 1.0);
        double Ecr = avgEnergyLoss::Ecr(E0, idHad, pdt);
        double x0 = Ecr/E0;
        
        double x = u + x0;
        double E = Ecr/x;
        return E;
    }
    
    double chLeptonExponent(const int idLep, Pythia8::ParticleData* pdt) {
        double E = chLeptonELossRate;
        return (timeConv*pdt->tau0(idLep)) * E / pdt->m0(idLep);
    }
    
    double E_leptonic(double E0, const int idLep, Pythia8::ParticleData* pdt) {
        double p = chLeptonExponent(idLep, pdt);
        double x = powerDistributionRandom(0, 1, p);
        
        //The E should be between [m0...E0] with m0 being the rest mass and E0 the initial energy
        double E = (pdt->m0(idLep)) + (E0-pdt->m0(idLep))*x;
        return E;
    }
}

using namespace Pythia8;

class SubDecayHandler : public DecayHandler {
public:
    
    SubDecayHandler(ParticleData* pdtPtrIn, Rndm* rndmPtrIn) {
        pdtPtr = pdtPtrIn; rndmPtr = rndmPtrIn;
    }
    
    bool decay(vector<int>& idProd, vector<double>& mProd,
               vector<Vec4>& pProd, int iDec, const Event& event);
    
    void addHandler(DecayHandler* handler, const std::vector<int>& particles);
    
    const std::vector<int> getHandledParticles();
    
    //~SubDecayHandler();
    
    
protected:
    // Pointer to the particle data table.
    ParticleData* pdtPtr;
    
    // Pointer to the random number generator.
    Rndm* rndmPtr;
    
    std::map<const int, DecayHandler*> decayMap;
    std::vector<DecayHandler*> decayHandlers;
    
};

//SubDecayHandler::~SubDecayHandler() {
//}

void SubDecayHandler::addHandler(DecayHandler* handler, const std::vector<int>& particles) {
    decayHandlers.push_back(handler);
    //for(auto& p : particles) {
    for(auto it=particles.begin(); it!=particles.end(); it++) {
        (this->decayMap)[(const int)*it] = handler;
        std::cout << "Adding " << *it << " to decay list." << std::endl;
    }
    return;
}

bool SubDecayHandler::decay(vector<int>& idProd, vector<double>& mProd, vector<Vec4>& pProd, int iDec, const Event& event) {
    int id = event[iDec].id();
    
    if (this->decayMap.find(id) == this->decayMap.end())
        return false;
    //cout << "Decaying n=" << iDec << endl;
    bool ret = (this->decayMap[id])->decay(idProd, mProd, pProd, iDec, event);
    //if (gPythia!=0) gPythia->event.list();
    return ret;
}

const std::vector<int> SubDecayHandler::getHandledParticles() {
    std::vector<int> out;
    for (auto it = this->decayMap.begin(); it != this->decayMap.end(); it++) {
        out.push_back(it->first);
    }
    return out;
}


class EnergyLossDecay : public DecayHandler {
public:
    
    EnergyLossDecay(ParticleData* pdtPtrIn, Rndm* rndmPtrIn) {
        pdtPtr = pdtPtrIn; rndmPtr = rndmPtrIn;
    }
    
    bool decay(vector<int>& idProd, vector<double>& mProd,
               vector<Vec4>& pProd, int iDec, const Event& event);
    
protected:
    
    virtual Vec4 energyLoss(const Vec4& p4, const int id, const int iDec, const Event& event) = 0;
    
    // Pointer to the particle data table.
    ParticleData* pdtPtr;
    
    // Pointer to the random number generator.
    Rndm* rndmPtr;
    
    //TH1D* hEAfterLoss = 0;
    //TH1D* hEBeforeLoss = 0;
    //std::map<const unsigned int, TH2D*> ELossHistMap;
    //std::map<const unsigned int, TH1D*> EScaleFactorHistMap;
    
    ~EnergyLossDecay() {};
    
    Vec4 newP4(const Vec4& p4_0, double E_new, const int id);
    
};

Vec4 EnergyLossDecay::newP4(const Vec4& p4_0, double E_1, const int id) {
    double p_abs_1 = sqrt( fabs(pow(E_1,2) - pow(pdtPtr->m0(id),2)) );
    double sf = p_abs_1/p4_0.pAbs();
    Vec4 p4_1(p4_0);
    p4_1.rescale3(sf);
    p4_1.e(E_1);
    return p4_1;
    //
    //    double sf = E_1/p4_0.e();
    //    Vec4 p4_1(p4_0);
    //    p4_1.rescale3(sf);
    //    double sig = 0.0;
    //    if (p4_0.e()>0) sig=1.0;
    //    else sig=-1.0;
    //    p4_1.e( sqrt( pow(pdtPtr->m0(id),2) - p4_1.pAbs2() ) );
    //    return p4_1;
}

bool EnergyLossDecay::decay(vector<int>& idProd, vector<double>& mProd,
                            vector<Vec4>& pProd, int iDec, const Event& event) {
    
    /*
     //Check for lifetime. FIXME: is this needed in Pythia or does Pythia
     //do it already before calling the decay method?
     if (event[iDec].tau() < event[iDec].tau0()) {
     //pProd.push_back(p4);
     #ifdef NDEBUG
     cout << "Lifetime not exceeded: " << event[iDec].tau() << " < " << event[iDec].tau0() << "\n";
     #endif
     return true;
     }
     */
    //std::cout << "Event size: " << event.size() << std::endl;
    
    const unsigned int absId = abs(idProd[0]);
    
    //TH2D* hELoss = 0;
    /*if (ELossHistMap.find(absId) == ELossHistMap.end()) {
     stringstream ss;
     ss << "ELossIdAbs" << absId;
     //ELossHistMap[absId] = new TH2D(ss.str().c_str(), ss.str().c_str(), 1000, 0, 5000, 1000, 0, 5000);
     }*/
    //hELoss = ELossHistMap[absId];
    
    //TH1D* hESF = 0;
    /*if (EScaleFactorHistMap.find(absId) == EScaleFactorHistMap.end()) {
     stringstream ss;
     ss << "ESFIdAbs" << absId;
     //EScaleFactorHistMap[absId] = new TH1D(ss.str().c_str(), ss.str().c_str(), 1000, 0, 1);
     }*/
    //hESF = EScaleFactorHistMap[absId];
    
    //Already decayed by external handler
    if (event[iDec].statusAbs() == 93 || event[iDec].statusAbs() == 94) {
        return false;
    }
    
    int id = idProd[0];
    double m = mProd[0];
    Vec4 p4 = pProd[0];
    
    double tau = event[iDec].tau();
    double tau0 = event[iDec].tau0();
    
    Vec4 p4_out = energyLoss(p4, id, iDec, event);
    //if (p4_out.pAbs2() > 0.99*p4.pAbs2()) {
    //    return true;
    //}
    idProd.push_back(id);
    mProd.push_back(m);
    pProd.push_back(p4_out);
    
    //Create fake graviton to respect energy conservation
    idProd.push_back(39);
    mProd.push_back(0);
    pProd.push_back(p4-p4_out);
    //hELoss->Fill(p4.e(), p4_out.e());
    //hESF->Fill(p4_out.e()/p4.e());
    
    //This particle decayed successfully externally
    return true;
    
}

// B or C hadrons lose energy in a continous way, based on the initial energy
// and the average energy loss. Based on Ritz-Seckel 2.5 (15)-(18)
class HeavyHadronDecayAverage : public EnergyLossDecay {
    
public:
    HeavyHadronDecayAverage(ParticleData* pdtPtrIn, Rndm* rndmPtrIn)
    : EnergyLossDecay(pdtPtrIn, rndmPtrIn) {
    }
    
private:
    //TH1D* h_sf;
    
protected:
    Vec4 energyLoss(const Vec4& p4, const int id, const int iDec, const Event& event) {
        double E_new = avgEnergyLoss::E(p4.e(), id, pdtPtr);
        Vec4 p4_out = newP4(p4, E_new, id);
        return p4_out;
    }
};

class HeavyHadronDecayProbabilistic : public EnergyLossDecay {
    
public:
    HeavyHadronDecayProbabilistic(ParticleData* pdtPtrIn, Rndm* rndmPtrIn)
    : EnergyLossDecay(pdtPtrIn, rndmPtrIn) {
    }
    
private:
    //TH1D* h_sf;
    
protected:
    Vec4 energyLoss(const Vec4& p4, const int id, const int iDec, const Event& event) {
        double E_new = energyLossDistributions::E_hadronic(p4.e(), id, pdtPtr);
        Vec4 p4_out = newP4(p4, E_new, id);
        return p4_out;
    }
};

//Light hadrons just stop
class LHadronDecayAverage : public EnergyLossDecay {
    
public:
    LHadronDecayAverage(ParticleData* pdtPtrIn, Rndm* rndmPtrIn)
    : EnergyLossDecay(pdtPtrIn, rndmPtrIn) {
    }
    
protected:
    Vec4 energyLoss(const Vec4& p4, const int id, const int iDec, const Event& event) {
        Vec4 p4_out(0, 0, 0, pdtPtr->m0(id));
        return p4_out;
    }
};

class CHLeptonDecayAverage : public EnergyLossDecay {
    
public:
    CHLeptonDecayAverage(ParticleData* pdtPtrIn, Rndm* rndmPtrIn)
    : EnergyLossDecay(pdtPtrIn, rndmPtrIn) {
    }
    
protected:
    Vec4 energyLoss(const Vec4& p4, const int id, const int iDec, const Event& event) {
        double p = energyLossDistributions::chLeptonExponent(id, pdtPtr);
        
        double E_new = pdtPtr->m0(id) + (p4.e() - pdtPtr->m0(id)) / (1.0+p);
        Vec4 p4_out = newP4(p4, E_new, id);
        return p4_out;
    }
};

class CHLeptonDecayProbabilistic : public EnergyLossDecay {
    
public:
    CHLeptonDecayProbabilistic(ParticleData* pdtPtrIn, Rndm* rndmPtrIn)
    : EnergyLossDecay(pdtPtrIn, rndmPtrIn) {
    }
    
private:
    //TH1D* h_sf;
    
protected:
    Vec4 energyLoss(const Vec4& p4, const int id, const int iDec, const Event& event) {
        double E_new = energyLossDistributions::E_leptonic(p4.e(), id, pdtPtr);
        Vec4 p4_out = newP4(p4, E_new, id);
        return p4_out;
    }
};



// A derived class for (e+ e- ->) GenericResonance -> various final states.
class Sigma1GenRes : public Sigma1Process {
    
public:
    
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

TDirectory* subDir(TDirectory* f, const char* prefix, const int val) {
    stringstream ss;
    ss << prefix << "_" << val;
    TDirectory* out = f->mkdir(ss.str().c_str());
    //f.cd(ss.str().c_str());
    out->cd();
    return out;
}

int main(int argc, char **argv) {
    cout << "ROOTSYS=" << getenv("ROOTSYS") << "\n";
    gROOT->ProcessLine(".L loader.C+");
    /*
     const std::vector<unsigned int> bHadrons({
     521,511,531,5122,5232,5132,5332
     });
     //{211,321,310,130,311,2112,3122,3222,3112,3322,3312,3334}
     const std::vector<unsigned int> cHadrons({
     411,421,431,4122,4232,4132,4332
     });
     const std::vector<unsigned int> lHadrons({
     211,321,310,130,311,2112,3122,3222,3112,3322,3312,3334
     });
     */
    
    std::vector<unsigned int> bHadrons;
    bHadrons.push_back(512);
    bHadrons.push_back(511); //B0
    bHadrons.push_back(531); //B_s0
    bHadrons.push_back(5122); //Lambda_b0
    bHadrons.push_back(5232); //Xi_b0
    bHadrons.push_back(5132); //Xi_b-
    bHadrons.push_back(5332); //Omega_b-
    
    std::vector<unsigned int> cHadrons;
    cHadrons.push_back(411);
    cHadrons.push_back(421);
    cHadrons.push_back(431);
    cHadrons.push_back(4122);
    cHadrons.push_back(4232);
    cHadrons.push_back(4132);
    cHadrons.push_back(4332);
    
    std::vector<unsigned int> lHadrons;
    lHadrons.push_back(211); //pi+
    lHadrons.push_back(321); //K+
    lHadrons.push_back(310); //K_S0
    lHadrons.push_back(130); //K_L0
    lHadrons.push_back(311); //K0
    lHadrons.push_back(2112); //n0
    lHadrons.push_back(3122); //Lambda0
    lHadrons.push_back(3222);
    lHadrons.push_back(3112);
    lHadrons.push_back(3322);
    lHadrons.push_back(3312);
    lHadrons.push_back(3334);
    
    std::vector<unsigned int> chLeptons;
    chLeptons.push_back(13);
    chLeptons.push_back(15);
    
    //Set up the GSL random number generator from the environment variables
    //GSL_RNG_SEED=123
    //GSL_RNG_TYPE=mrg <- multiple recursive generator
    gsl_rng_env_setup();
    randomGeneratorType = gsl_rng_default;
    rng = gsl_rng_alloc (randomGeneratorType);
    seedRandom();
    
    if (argc != 8 ) {
        cout << "Usage: ./gen part DMmass output.root params.card bchad lhad chlep" << endl;
        return 1;
    }
    gsl_set_error_handler(&gslErrorHandler);
    
    int partId, apartId;
    double dmMass;
    partId = atoi(argv[1]);
    dmMass = atof(argv[2]);
    
    //Flags that control if BC and/or L should be reduced in energy before decaying
    int hHadronELossInstruction = atoi(argv[5]);
    int lHadronELossInstruction = atoi(argv[6]);
    int chLeptonELossInstruction = atoi(argv[7]);
    
    if (partId == 23 || partId == 25) apartId = partId; else apartId = -partId;
    
    //must declare on heap not stack
    Pythia* _pythia = new Pythia();
    gPythia = _pythia;
    Pythia& pythia = *_pythia;
    
    // A class to generate the fictitious resonance initial state.
    SigmaProcess* sigma1GenRes = new Sigma1GenRes();
    pythia.setSigmaPtr( sigma1GenRes);
    pythia.readFile(argv[4]);
    
    char ch[60];
    
    if (partId != 1)
        sprintf(ch,"999999:onIfAll %d %d", partId, apartId); //turn on (id -id)
    else
        sprintf(ch,"999999:onIfAny 1 2 3"); //Turn on light quarks
    
    pythia.readString("999999:onMode = off");
    pythia.readString(ch);
    
    // Set beam CM energy to 2*DMmass for annihilation. This is equivalent to decay of twice as massive DM particle
    sprintf(ch,"Beams:eCM = %1.2f",2*dmMass);
    pythia.readString(ch);
    // Initialization.
    
    // Extract settings to be used in the main program.
    int nEvent = pythia.mode("Main:numberOfEvents");
    int nList = pythia.mode("Main:numberToList");
    int nShow = pythia.mode("Main:timesToShow");
    int nAbort = pythia.mode("Main:timesAllowErrors");
    bool showCS = pythia.flag("Main:showChangedSettings");
    bool showCPD = pythia.flag("Main:showChangedParticleData");
    pythia.readString("ParticleDecays:limitTau = off");
    
    TFile f(argv[3],"RECREATE");
    
    std::stringstream ss;
    ss << "energyLoss" << "_hhad_" << hHadronELossInstruction << "_lhad_" << lHadronELossInstruction << "_chlep_" << chLeptonELossInstruction;
    
    TDirectory* massDir = subDir(&f, "mass", (int)dmMass);
    TDirectory* particleDir = subDir(massDir, "particle", (int)partId);
    
    TDirectory* energyLossDir = particleDir->mkdir(ss.str().c_str());
    energyLossDir->cd();
    
    
    //Add energy loss decay for b and c hadrons
    SubDecayHandler* mainDecayHandler = new SubDecayHandler(&pythia.particleData, &pythia.rndm);
    
    if (hHadronELossInstruction != 0) {
        DecayHandler* handleBCHadDecays = 0;
        if (hHadronELossInstruction == 1) {
            cout << "Decaying heavy hadron energy by HeavyHadronDecayAverage" << endl;
            handleBCHadDecays = new HeavyHadronDecayAverage(
                                                            &pythia.particleData, &pythia.rndm
                                                            );
            
        } else if (hHadronELossInstruction == 2) {
            cout << "Decaying heavy hadron energy by HeavyHadronDecayProbabilistic" << endl;
            handleBCHadDecays = new HeavyHadronDecayProbabilistic(
                                                                  &pythia.particleData, &pythia.rndm
                                                                  );
        } else {
            std::cerr << "hHadronELossInstruction value not recognized" << std::endl;
            exit(1);
        }
        
        vector<int> handledBCHadrons;
        for(auto it=bHadrons.begin(); it!=bHadrons.end(); it++) {
            handledBCHadrons.push_back((const int)*it);
            handledBCHadrons.push_back(-(const int)*it);
        }
        for(auto it = cHadrons.begin(); it != cHadrons.end(); it++) {
            handledBCHadrons.push_back((const int)*it);
            handledBCHadrons.push_back(-(const int)*it);
        }
        mainDecayHandler->addHandler(handleBCHadDecays, handledBCHadrons);
    }
    
    if (lHadronELossInstruction != 0) {
        DecayHandler* handleLHadDecays = 0;
        if (lHadronELossInstruction == 1) {
            cout << "Decaying light hadron energy by LHadronDecayAverage" << endl;
            
            handleLHadDecays = new LHadronDecayAverage(&pythia.particleData,
                                                       &pythia.rndm);
        } else {
            std::cerr << "lHadronELossInstruction value not recognized" << std::endl;
            exit(1);
        }
        
        vector<int> handledLHadrons;
        for(auto it=lHadrons.begin(); it!=lHadrons.end(); it++) {
            handledLHadrons.push_back((const int)*it);
            handledLHadrons.push_back(-(const int)*it);
        }
        mainDecayHandler->addHandler(handleLHadDecays, handledLHadrons);
    }
    
    if (chLeptonELossInstruction != 0) {
        DecayHandler* handleChLepDecays = 0;
        if (chLeptonELossInstruction == 1) {
            cout << "Decaying charged lepton energy by CHLeptonDecayAverage" << endl;
            
            handleChLepDecays = new CHLeptonDecayAverage(&pythia.particleData,
                                                         &pythia.rndm);
            
        } else if (chLeptonELossInstruction == 2) {
            cout << "Decaying charged lepton energy by CHLeptonDecayProbabilistic" << endl;
            handleChLepDecays = new CHLeptonDecayProbabilistic(&pythia.particleData,
                                                               &pythia.rndm);
            
        } else {
            std::cerr << "chLeptonELossInstruction value not recognized" << std::endl;
            exit(1);
        }
        
        vector<int> handledChLeptons;
        for(auto it=chLeptons.begin(); it!= chLeptons.end(); it++) {
            handledChLeptons.push_back((const int)*it);
            handledChLeptons.push_back(-(const int)*it);
        }
        mainDecayHandler->addHandler(handleChLepDecays, handledChLeptons);
    }
    pythia.setDecayPtr((DecayHandler*)mainDecayHandler, mainDecayHandler->getHandledParticles());
    
    
    pythia.init();
    cout << "Generating " << nEvent << " events of DM with mass " << dmMass << " GeV annihilating to " << partId << endl;
    //if (showCS)  pythia.settings.listChanged();
    //if (showCPD) pythia.particleData.listChanged();
    
    const unsigned int nBins = 300;
    
    TH1I *hEventStatus = new TH1I("eventStatus","Event status distribution",2,0,2);
    
    TH1D *hantip = new TH1D("antip","Antiproton distribution",nBins,-9,0);
    TH1D *hantin = new TH1D("antin","Antineutron distribution",nBins,-9,0);
    TH1D *hproton = new TH1D("proton","Proton distribution",nBins,-9,0);
    TH1D *hneutron = new TH1D("neutron","Neutron distribution",nBins,-9,0);
    
    //TH2D *hantid = new TH2D("antid","Antideteron distribution",nBins,-9,0,400,0,0.4);
    TH1D *hel = new TH1D("el","Electron distribution",nBins,-9,0);
    
    TH1D *hnuel = new TH1D("nuel","Electron nu distribution",nBins,-9,0);
    //TH1D *hnuelE = new TH1D("nuelE","Electron nu energy distribution",nBins,0,10000);
    
    TH1D *hnumu = new TH1D("numu","Muon nu distribution",nBins,-9,0);
    //TH1D *hnumuE = new TH1D("numuE","Muon nu energy distribution",nBins,0,10000);
    
    TH1D *hnutau = new TH1D("nutau","Tau nu distribution",nBins,-9,0);
    //TH1D *hnutauE = new TH1D("nutauE","Tau nu energy distribution",nBins,0,10000);
    
    TH1D *hgam = new TH1D("gam","Gamma distribution",nBins,-9,0);
    //TH1D *hBHad = new TH1D("bHad","b Hadron energy distribution",nBins,0,200);
    //TH1D *hCHad = new TH1D("cHad","c Hadron energy distribution",nBins,0,150);
    //TH1D *hLHad = new TH1D("lHad","l Hadron energy distribution",nBins,0,30);
    //hEnergySF = new TH1D("ESF","Energy loss scale factor",nBins,0,1);
    
    // vector<Particle> antip;
    // vector<Particle> antin;
    
    //std::vector<Neutrino> neutrinos;
    //std::vector<SolarNu::Particle*> * p_neutrinos = new std::vector<SolarNu::Particle*>();
    
    #ifdef NDEBUG
    TTree* tree = new TTree("events", "events");
    std::vector<float> *p_energy = new std::vector<float>();
    std::vector<int> *p_id = new std::vector<int>();
    std::vector<int> *p_parent = new std::vector<int>();
    TBranch* br1 = tree->Branch("particles_E", "std::vector<float>", &p_energy);
    TBranch* br2 = tree->Branch("particles_id", "std::vector<int>", &p_id);
    TBranch* br3 = tree->Branch("particles_parent", "std::vector<int>", &p_parent);
    
    //TBranch* br = tree->Branch("neutrinos", "std::vector<Particle>", &p_neutrinos);
    if (br1==0 || br2==0 || br3==0) {
        std::cerr << "Could not create branch, exiting" << std::endl;;
        exit(1);
    }
    #endif

    
    int iAbort = 0;
    std::cout << "Starting event loop" << std::endl;
    for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    
        #ifdef NDEBUG
        p_energy->clear();
        p_id->clear();
        p_parent->clear();
        #endif
        
        // Generate events. Quit if many failures.
        if (!pythia.next()) {
            if (++iAbort < nAbort) {
                hEventStatus->Fill(1);
                PythiaErrCount++;
                pythia.event.list();
                
                continue;
            }
            cout << " Event generation aborted prematurely, owing to error!\n";
            break;
        }
        hEventStatus->Fill(0); //Fill with return code 0: successful event generation
        
        
        //Show how far along the run is
        if(nShow>0 && iEvent%nShow==0)
            cout << "Processed " << iEvent << " events" << endl;
        
        //List the events
        if (iEvent < nList) {
            //pythia.info.list();
            //pythia.event.list();
        }
        
        // FIXME: This decay hack is from the old method and probably can be done
        // instead using a specific decay handler for leptons.
        // --JP
        
        // Here's where we have to decay the particles that were still left intact
        // 1. loop over all particles and add at the end of the event copies of those while they are standing still
        //int nn = pythia.event.size();
        //for (int i=0; i < nn; ++i)
        /* //DEPRECATED(?)
         if (pythia.event[i].isFinal()) {
         int id = pythia.event[i].id();
         int idAbs = abs(id);
         double m = pythia.event[i].mass();
         
         //mu, tau, c, b, t, Z0, W+-, H0
         if (idAbs == 13 || idAbs == 15 || idAbs == 4 || idAbs == 5 || idAbs == 6 || idAbs == 23 || idAbs == 24 || idAbs == 25) {
         // If it's one of the relevant particles, then add a respective particle to the event, but at rest
         pythia.event.append(id,1,0,0,0.,0.,0.,m,m);
         //cout << " New particle: " << ni << " mayDecay = " << pythia.event[ni].mayDecay() << " tau=" << pythia.event[ni].tau() << endl;
         }
         }
         */
        
        //cout << "With my additions it's " << pythia.event.size() << " particles" << endl;
        // Force the new particles to decay
        //pythia.readString("ParticleDecays:limitTau = off");
        //pythia.moreDecays();
        //pythia.readString("ParticleDecays:limitTau = on");
        //cout << "And after decays it's  " << pythia.event.size() << " particles" << endl;
        
        // List first few events.
        
        //Get spectra of protons and neutrons
        for (int i = 0; i < pythia.event.size(); ++i) {
            if (pythia.event[i].isFinal()) {
                int id = pythia.event[i].id();
                //int idAbs = abs(id);
                double x = log10((pythia.event[i].e()-pythia.event[i].m())/dmMass);
                if (id == -2212) {
                    //antip.push_back(pythia.event[i]);
                    hantip->Fill(x);
                }
                else if (id == 2212) {
                    hproton->Fill(x);
                }
                
                //antineutrons
                else if (id == -2112) {
                    //antin.push_back(pythia.event[i]);
                    hantin->Fill(x);
                }
                if (id == 2112) {
                    hneutron->Fill(x);
                }
            }
        }
        
        //decay neutrons
        pythia.particleData.mayDecay(2112,true);
        pythia.moreDecays();
        pythia.particleData.mayDecay(2112,false);
        
        
        //Get spectra of final stable particles
        for (int i = 0; i < pythia.event.size(); ++i) {
            int id = pythia.event[i].id();
            int idAbs = abs(id);
            
            if (pythia.event[i].isFinal()) {
                double x = log10((pythia.event[i].e()-pythia.event[i].m())/dmMass);
                if (idAbs == 11) {
                    hel->Fill(x);
                }
                if (idAbs == 12) {
                    hnuel->Fill(x);
                    //hnuelE->Fill(pythia.event[i].e());
                    //p_neutrinos->push_back(new SolarNu::Particle(pythia.event[i].e(), pythia.event[i].id() , pythia.event[i].mother1()));
                    
                    #ifdef NDEBUG
                    p_energy->push_back(pythia.event[i].e());
                    p_id->push_back(pythia.event[i].id());
                    p_parent->push_back(pythia.event[pythia.event[i].mother1()].id());
                    #endif
                }
                if (idAbs == 14) {
                    hnumu->Fill(x);
                    
                    #ifdef NDEBUG
                    p_energy->push_back(pythia.event[i].e());
                    p_id->push_back(pythia.event[i].id());
                    p_parent->push_back(pythia.event[pythia.event[i].mother1()].id());
                    #endif
                    //hnumuE->Fill(pythia.event[i].e());
                }
                if (idAbs == 16) {
                    hnutau->Fill(x);
                    
                    #ifdef NDEBUG
                    p_energy->push_back(pythia.event[i].e());
                    p_id->push_back(pythia.event[i].id());
                    p_parent->push_back(pythia.event[pythia.event[i].mother1()].id());
                    #endif
                    //hnutauE->Fill(pythia.event[i].e());
                }
                if (id == 22) {
                    hgam->Fill(x);
                    
                    #ifdef NDEBUG
                    p_energy->push_back(pythia.event[i].e());
                    p_id->push_back(pythia.event[i].id());
                    p_parent->push_back(pythia.event[pythia.event[i].mother1()].id());
                    #endif
                }
                if (id == -2212) {
                    hantip->Fill(x);
                }
            }
            
            /*
             //Fill histograms for particles that have been decayed by pythia
             //even if they are not "final" particles
             if(pythia.event[i].statusAbs()==91) {
             if(std::find(bHadrons.begin(), bHadrons.end(), idAbs) != bHadrons.end()) {
             hBHad->Fill(pythia.event[i].e());
             }
             if(std::find(cHadrons.begin(), cHadrons.end(), idAbs) != cHadrons.end()) {
             hCHad->Fill(pythia.event[i].e());
             }
             if(std::find(lHadrons.begin(), lHadrons.end(), idAbs) != lHadrons.end()) {
             hLHad->Fill(pythia.event[i].e());
             }
             }
             */
            
        }
        
        /*
         // Loop over anti-partons and fill histograms
         for (vector<Particle>::iterator ap = antip.begin(); ap != antip.end(); ap++)
         for (vector<Particle>::iterator an = antin.begin(); an != antin.end(); an++) {
         Vec4 d = ap->p() - an->p();
         Vec4 d2 = ap->p() + an->p();
         double y = sqrt(pow(d.px(),2)+pow(d.py(),2)+pow(d.pz(),2)-pow(ap->e()-an->e(),2));
         //double x = log10((d2.e()-ap->m()-an->m())/dmMass);
         double x = 0;
         if (y < 0.16 && ap->status() > 0 && an->status() > 0) {
         hantid->Fill(x,y);
         ap->statusNeg();
         an->statusNeg();
         }
         }
         */
        // clean up
        //antip.clear();
        //antin.clear();
        // End of event loop.
        //br->Fill();
        #ifdef NDEBUG
        tree->Fill();
        #endif
    }
    cout << "Writing output from " << nEvent << " events." << endl;
    cout << "GSL error count: " << gslErrCount << endl;
    cout << "Pythia error count: " << PythiaErrCount << endl;
    f.cd();
    f.Write();
    f.Close();
    
    return 0;
}
