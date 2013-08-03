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
#include <TStopwatch.h>
#include <TH1.h>
#include <TH2.h>
#include <Pythia.h>
#include "particle.h"
#include "TROOT.h"

//global pointer to the Pythia object
Pythia8::Pythia *gPythia = 0;

//Enable this to switch on event particle list generation and other debugging
//#define NDEBUG

//Set up the GSL random number generation
const gsl_rng_type *randomGeneratorType = 0;
const gsl_rng *rng = 0;

unsigned int gslErrCount = 0;
unsigned int PythiaErrCount = 0;

void seedRandom();
void seedRandom()
{
    ifstream f("/dev/urandom");
    unsigned short seed = 0;
    if (f.good())
    {

        //read the random seed from /dev/urandom
        f.read(reinterpret_cast<char *>(&seed), sizeof(seed));
        cout << "Setting GSL random seed to " << seed << endl;
        f.close();

    }
    else
    {
        cerr << "Failed to open stream from /dev/urandom." << endl;
        exit(1);
    }
    gsl_rng_set(rng, seed);
    return;
}


//Returns the pdgId of the primary quark in the hadron by getting the first digit
// of the absolute value.
int idQuark(int idHad)
{

    std::ostringstream os;
    os << abs(idHad);
    const std::string digits = os.str().substr(0, 1);
    const char f = digits[0];
    int first = 0;
    std::istringstream ( digits ) >> first;
    //std::cerr << "idHad=" << idHad << " idQ=" << first << " f='" << digits  << "'" << std::endl;
    return first;
}

void gslErrorHandler(const char *reason, const char *file, int line, int gsl_errno);
void gslErrorHandler(const char *reason, const char *file, int line, int gsl_errno)
{
    gslErrCount++;
    cerr << "GSL error: " << gsl_errno << " in file:line " <<
         file  << ":" << line << " with reason '" << reason << "'\n";
}

//samples a number from the power distribution
double powerDistributionRandom(double xu, double xl, double n)
{
    double y = gsl_rng_uniform(rng);
    //    double x = pow(
    //                   (pow(xu, n + 1.0) - pow(xl, n + 1.0))*y + pow(xl, n + 1.0),
    //                   n/(n+1.0)
    //                   );
    double x = pow(y, n);

    return x;
}

namespace energyLossDistributions
{
double chLeptonExponent(const int idLep, Pythia8::ParticleData *pdt);
};

// 1 [mm/(c m/s)] = timeConv [s].
const double timeConv = 3.33564095E-12;

//Calculates the average energy loss that a B or C hadron suffers in
//dense material. Implemented as in http://arxiv.org/pdf/hep-ph/0506298v5.pdf
//formula 8, 9
namespace avgEnergyLoss
{

//typical interaction time for hadrons in the Sun
//double tint = 2.5E-11; // Page 12 of Strumia, Cirelli et al
double tint = 3.5E-11; // (14) in Ritz & Seckel

//quark/hadron mass ratio
double x(int idQ, int idHad, Pythia8::ParticleData *pdt);
double x(int idQ, int idHad, Pythia8::ParticleData *pdt)
{
    if (idQ == 4 || idQ == 5) //c or b
    {
        return pdt->m0(idQ) / pdt->m0(idHad);
    }
    else
    {
        std::cerr << "Unknown idQ=" << idQ << std::endl;
        throw 1;
    }
    return GSL_NAN;
}

double z(int idQ);
double z(int idQ)
{
    if (idQ == 4)  //c
    {
        return 0.6;
    }
    else if (idQ == 5)     //b
    {
        return 0.8;
    }
    return GSL_NAN;
}

//Stopping time in seconds
double t_stop(double E0, int idHad, Pythia8::ParticleData *pdt)
{
    int idQ = idQuark(idHad);
    double Z = x(idQ, idHad, pdt) * z(idQ);
    bool isBaryon = pdt->spinType(idHad) % 2;
    double tstop = tint / (1.0 - Z);
    // The scattering cross-section is different for baryons and mesons, hence the stopping time changes
    if (isBaryon)
        tstop = tstop / 2.5; // section 2.5 of Ritz & Seckel
    return tstop;
}


double Ecr(double E0, int idHad, Pythia8::ParticleData *pdt)
{
    double tdec = timeConv * pdt->tau0(idHad);
    double tstop = t_stop(E0, idHad, pdt);
    double Ec = (pdt->m0(idHad)) * tstop / tdec;
    return Ec;
}

double E_hadronic(double E0, int idHad, Pythia8::ParticleData *pdt)
{
    double Ec = Ecr(E0, idHad, pdt);
    double E_kin = E0 - pdt->m0(idHad);

    double _x = Ec / E_kin;

    double E1 = TMath::QuietNaN();
    if (!std::isnan(_x))
    {
        double f = gsl_sf_gamma_inc(0.0, _x); //Integral as incomplete gamma function from gsl
        if (f != GSL_ERANGE)   //no over/underflow
        {
            E1 = Ec * exp(_x) * f;
        }
    }
    E1 = pdt->m0(idHad) + E1;
    return E1;
}

double E_leptonic(double E0, int idLep, Pythia8::ParticleData *pdt)
{
    double p = energyLossDistributions::chLeptonExponent(idLep, pdt);
    double E_kin = E0 - pdt->m0(idLep);
    double E_new = pdt->m0(idLep) + E_kin / (1.0 + p);
    return E_new;
}

double E_stop(double E0, int id, Pythia8::ParticleData *pdt)
{
    return pdt->m0(id);
}

}

namespace energyLossDistributions
{

//Charged lepton energy loss rate in the Sun [GeV/s].
const double chLeptonELossRate = 0.8E10;

//Draws the energy loss from the exponential distribution p(x)=\exp(x_0 - x) where
// x = E_cr / E and x_0 = E_cr / E_0 with E_cr depending on the hadron species

//FIXME: draw value for x from gsl_ran_exponential
double E_hadronic(double E0, int idHad, Pythia8::ParticleData *pdt)
{

    // The PDF for x is p(x)=\exp(x_0 - x) with normalization \int_x0^\infty p(x) dx = 1
    // Thus by change of variables u = x_0 - x we have an exponential distribution
    // \exp(u) with mean 1
    double u = gsl_ran_exponential(rng, 1.0);
    double Ecr = avgEnergyLoss::Ecr(E0, idHad, pdt);
    if (std::isnan(Ecr))
    {
        std::cerr << "Ecr=nan for " << idHad << " E0=" << E0 << std::endl;
        throw 1;
    }

    double E_kin = E0 - pdt->m0(idHad);
    double x0 = Ecr / E_kin;
    double x = u + x0;
    double E = pdt->m0(idHad) + Ecr / x;
    return E;
}

double chLeptonExponent(const int idLep, Pythia8::ParticleData *pdt)
{
    double E = chLeptonELossRate;
    return (timeConv * pdt->tau0(idLep)) * E / pdt->m0(idLep);
}

//Returns the total energy of a charged lepton undergoing energy loss
//E0 - total energy before energy losses (time-component of p4)
//idLep - absolute value of the pdgID of the lepton
//pdt - a pointer to the Pythia particle data table
double E_leptonic(double E0, const int idLep, Pythia8::ParticleData *pdt)
{
    double p = chLeptonExponent(idLep, pdt);
    double x = powerDistributionRandom(0, 1, p);

    //The E should be between [m0...E0] with m0 being the rest mass and E0 the initial energy
    //double E = (pdt->m0(idLep)) + (E0-pdt->m0(idLep))*x;

    double E_kin = E0 - pdt->m0(idLep);
    double E1 = pdt->m0(idLep) + E_kin * x;
    return E1;
}
}

using namespace Pythia8;

//Pythia can only accept 1 external decay handler.
//Thus we need a handler that can differentiate between particles and call
//their respective handlers
class SubDecayHandler : public DecayHandler
{
public:

    SubDecayHandler(ParticleData *pdtPtrIn, Rndm *rndmPtrIn)
    {
        pdtPtr = pdtPtrIn; rndmPtr = rndmPtrIn;
    }

    bool decay(vector<int> &idProd, vector<double> &mProd,
               vector<Vec4> &pProd, int iDec, const Event &event);

    //Adds a handler that will handle the particles with the pdgids specified
    //by their pdg-ids
    void addHandler(DecayHandler *handler, const std::vector<int> &particles);

    //returns the list of pdg-ids decayed by this handler.
    const std::vector<int> getHandledParticles();

    //~SubDecayHandler();


protected:
    // Pointer to the particle data table.
    ParticleData *pdtPtr;

    // Pointer to the random number generator.
    Rndm *rndmPtr;

    //a pdgid <-> *handler map that associates with each particle a decay handler
    std::map<const int, DecayHandler *> decayMap;
    std::vector<DecayHandler *> decayHandlers;

};

void SubDecayHandler::addHandler(DecayHandler *handler, const std::vector<int> &particles)
{
    decayHandlers.push_back(handler);
    //for(auto& p : particles) {
    for (auto it = particles.begin(); it != particles.end(); it++)
    {
        (this->decayMap)[(const int)*it] = handler;
        std::cout << "Adding " << *it << " to decay list." << std::endl;
    }
    return;
}

//decays a particle with index iDec
bool SubDecayHandler::decay(vector<int> &idProd, vector<double> &mProd, vector<Vec4> &pProd, int iDec, const Event &event)
{
    int id = event[iDec].id();

    if (this->decayMap.find(id) == this->decayMap.end())
        return false;

    bool ret = (this->decayMap[id])->decay(idProd, mProd, pProd, iDec, event);
    //if (gPythia!=0) gPythia->event.list();
    return ret;
}

const std::vector<int> SubDecayHandler::getHandledParticles()
{
    std::vector<int> out;
    for (auto it = this->decayMap.begin(); it != this->decayMap.end(); it++)
    {
        out.push_back(it->first);
    }
    return out;
}


//A generic decay handler that will lose the energy of particles by radiating
//it away via fake gravitons to enforce energy conservation
class EnergyLossDecay : public DecayHandler
{
public:

    EnergyLossDecay(ParticleData *pdtPtrIn, Rndm *rndmPtrIn, std::function<double(double, int id, ParticleData *pdtPtrIn)> _energy_loss_func)
    {
        pdtPtr = pdtPtrIn; rndmPtr = rndmPtrIn;
        energy_loss_func = _energy_loss_func;

    }

    bool decay(vector<int> &idProd, vector<double> &mProd,
               vector<Vec4> &pProd, int iDec, const Event &event);

protected:

    //virtual Vec4 energyLoss(const Vec4& p4, const int id, const int iDec, const Event& event) = 0;

    // Pointer to the particle data table.
    ParticleData *pdtPtr;

    std::function<double(double, int id, ParticleData *pdtPtrIn)> energy_loss_func;

    // Pointer to the random number generator.
    Rndm *rndmPtr;

    ~EnergyLossDecay() {};

    //Calculates the rescaled quadrimomentum of the particle with pdgID id.
    Vec4 newP4(const Vec4 &p4_0, double E_new, const int id);

};

//This method calculates the rescaled quadrimomentum of a particle with pdgID id
//and initial momentum p4_0. The minimum of the final momentum p4_1 is specified by
//the particle mass.
Vec4 EnergyLossDecay::newP4(const Vec4 &p4_0, double E_1, const int id)
{
    double p_abs_squared_1 = pow(E_1, 2) - pow(pdtPtr->m0(id), 2);
    if (p_abs_squared_1 < 0.0)
    {
        cerr << "Negative square in newP4" << endl;
        p_abs_squared_1 = 0.0;
        //throw 1;
    }
    double p_abs_1 = sqrt(p_abs_squared_1);
    double sf = p_abs_1 / p4_0.pAbs();
    Vec4 p4_1(p4_0);
    if (sf >= 0)
    {
        p4_1.rescale3(sf);
        p4_1.e(E_1);
    }
    else
    {
        //cout << "not rescaling" << endl;
    }
    //double mcalc = p4_1.mCalc();
    //if (mcalc<0 || std::isnan<double>(mcalc)) {
    //    cerr << "p4 was nan" << endl;
    //    throw 1;
    //}
    return p4_1;
}

bool EnergyLossDecay::decay(vector<int> &idProd, vector<double> &mProd,
                            vector<Vec4> &pProd, int iDec, const Event &event)
{

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

    //const unsigned int absId = abs(idProd[0]);

    //Already decayed by external handler, return false to decay with Pythia
    if (event[iDec].statusAbs() == 93 || event[iDec].statusAbs() == 94)
    {
        return false;
    }

    //properties of the decaying particle
    int id = idProd[0];
    double m = mProd[0];
    Vec4 p4 = pProd[0];
    //double tau = event[iDec].tau();
    //double tau0 = event[iDec].tau0();

    //decay the particle to itself but with rescaled momentum
    double E1 = energy_loss_func(p4.e(), id, pdtPtr);
    Vec4 p4_out = newP4(p4, E1, id);
    idProd.push_back(id);
    mProd.push_back(m);
    pProd.push_back(p4_out);

    //Create fake graviton to respect energy conservation
    //NOTE: in 2-body decay, the second particle has fixed kinematics and may be off the mass shell. We don't care about this for gravitons.
    idProd.push_back(39);
    mProd.push_back(0);
    pProd.push_back(p4 - p4_out);

    return true;
}

// A derived class for (e+ e- ->) GenericResonance -> various final states.
class Sigma1GenRes : public Sigma1Process
{

public:

    Sigma1GenRes() {}

    // Evaluate sigmaHat(sHat): dummy unit cross section.
    virtual double sigmaHat()
    {
        return 1.;
    }

    // Select flavour. No colour or anticolour.
    virtual void setIdColAcol()
    {
        setId( -11, 11, 999999);
        setColAcol( 0, 0, 0, 0, 0, 0);
    }

    // Info on the subprocess.
    virtual string name()    const
    {
        return "GenericResonance";
    }
    virtual int    code()    const
    {
        return 9001;
    }
    virtual string inFlux()  const
    {
        return "ffbarSame";
    }

};

//Creates a subdirectory in the directory f with prefix_val
//Returns a pointer to the created directory
template <typename T=const int>
TDirectory* subDir(TDirectory* f, const char* prefix, T val) {
    stringstream ss;
    ss << prefix << "_" << val;
    TDirectory *out = f->mkdir(ss.str().c_str());
    //f.cd(ss.str().c_str());
    out->cd();
    return out;
}

int main(int argc, char **argv)
{
    cout << "ROOTSYS=" << getenv("ROOTSYS") << "\n";

    //Compile the dictionaries for the classes that are put to the TTree
    //    gROOT->ProcessLine(".L loader.C+");

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
    chLeptons.push_back(13); //mu
    chLeptons.push_back(15); //tau

    //Set up the GSL random number generator from the environment variables
    //GSL_RNG_TYPE=mrg <- multiple recursive generator
    gsl_rng_env_setup();
    randomGeneratorType = gsl_rng_default;
    rng = gsl_rng_alloc (randomGeneratorType);
    seedRandom();

    std::cout << argc << std::endl;
    if (!(argc == 7 || argc == 8)) {
        cout << "Usage: ./genSun.exe part DMmass output.root params.card bchad lhad chlep (NEv)" << endl;
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
    
    const char* cardFile = argv[4];
    if (partId == 23 || partId == 25) apartId = partId; else apartId = -partId;

    //must declare on heap (using new...) not stack to avoid crash
    Pythia *_pythia = new Pythia();
    gPythia = _pythia;
    Pythia &pythia = *_pythia;

    // A class to generate the fictitious resonance initial state.
    SigmaProcess *sigma1GenRes = new Sigma1GenRes();
    pythia.setSigmaPtr( sigma1GenRes);
    pythia.readFile(cardFile);
    
    char ch[60];

    if (partId != 1)
        sprintf(ch, "999999:onIfAll %d %d", partId, apartId); //turn on (id -id)
    else
        sprintf(ch, "999999:onIfAny 1 2 3"); //Turn on light quarks

    pythia.readString("999999:onMode = off");
    pythia.readString(ch);

    // Set beam CM energy to 2*DMmass for annihilation. This is equivalent to decay of twice as massive DM particle
    sprintf(ch, "Beams:eCM = %1.2f", 2 * dmMass);
    pythia.readString(ch);
    // Initialization.

    // Extract settings to be used in the main program.
    int nEvent = pythia.mode("Main:numberOfEvents");
    if (argc == 9) nEvent = atoi(argv[8]);
    int nList = pythia.mode("Main:numberToList");
    int nShow = pythia.mode("Main:timesToShow");
    int nAbort = pythia.mode("Main:timesAllowErrors");
    bool showCS = pythia.flag("Main:showChangedSettings");
    bool showCPD = pythia.flag("Main:showChangedParticleData");
    pythia.readString("ParticleDecays:limitTau = off");
    
    TFile f(argv[3],"RECREATE");
    
    //Create the subdirectories for the output
    std::stringstream ss;
    ss << "energyLoss" << "_hhad_" << hHadronELossInstruction << "_lhad_" << lHadronELossInstruction << "_chlep_" << chLeptonELossInstruction;
    TDirectory* cardDir = subDir<const char*>(&f, "card", cardFile);
    TDirectory* massDir = subDir(cardDir, "mass", (int)dmMass);
    TDirectory* particleDir = subDir(massDir, "particle", (int)partId);
    
    TDirectory* energyLossDir = particleDir->mkdir(ss.str().c_str());
    energyLossDir->cd();


    //Add energy loss decay for b and c hadrons
    SubDecayHandler *mainDecayHandler = new SubDecayHandler(&pythia.particleData, &pythia.rndm);

    if (hHadronELossInstruction != 0)
    {
        DecayHandler *handleBCHadDecays = 0;

        //average decay
        if (hHadronELossInstruction == 1)
        {
            cout << "Decaying heavy hadron energy by HeavyHadronDecayAverage" << endl;
            handleBCHadDecays = new EnergyLossDecay(
                &pythia.particleData, &pythia.rndm, avgEnergyLoss::E_hadronic);
            //Probabilistic decay
        }
        else if (hHadronELossInstruction == 2)
        {
            cout << "Decaying heavy hadron energy by HeavyHadronDecayProbabilistic" << endl;
            handleBCHadDecays = new EnergyLossDecay(
                &pythia.particleData, &pythia.rndm, energyLossDistributions::E_hadronic);
        }
        else
        {
            std::cerr << "hHadronELossInstruction value not recognized" << std::endl;
            exit(1);
        }

        //Create a list of the particle and anti-particle id-s that are decayed
        //by the heavy hadron mechanism
        vector<int> handledBCHadrons;
        for (auto it = bHadrons.begin(); it != bHadrons.end(); it++)
        {
            handledBCHadrons.push_back((const int)*it);
            handledBCHadrons.push_back(-(const int)*it);
        }
        for (auto it = cHadrons.begin(); it != cHadrons.end(); it++)
        {
            handledBCHadrons.push_back((const int)*it);
            handledBCHadrons.push_back(-(const int)*it);
        }
        mainDecayHandler->addHandler(handleBCHadDecays, handledBCHadrons);
    }

    if (lHadronELossInstruction != 0)
    {
        DecayHandler *handleLHadDecays = 0;

        //Average decay
        if (lHadronELossInstruction == 1)
        {
            cout << "Decaying light hadron energy by LHadronDecayAverage" << endl;

            handleLHadDecays = new EnergyLossDecay(&pythia.particleData,
                                                   &pythia.rndm,
                                                   avgEnergyLoss::E_stop
                                                  );
        }
        else
        {
            std::cerr << "lHadronELossInstruction value not recognized" << std::endl;
            exit(1);
        }

        vector<int> handledLHadrons;
        for (auto it = lHadrons.begin(); it != lHadrons.end(); it++)
        {
            handledLHadrons.push_back((const int)*it);
            handledLHadrons.push_back(-(const int)*it);
        }
        mainDecayHandler->addHandler(handleLHadDecays, handledLHadrons);
    }

    if (chLeptonELossInstruction != 0)
    {
        DecayHandler *handleChLepDecays = 0;

        //Average decay
        if (chLeptonELossInstruction == 1)
        {
            cout << "Decaying charged lepton energy by CHLeptonDecayAverage" << endl;

            handleChLepDecays = new EnergyLossDecay(
                &pythia.particleData,
                &pythia.rndm,
                avgEnergyLoss::E_leptonic
            );
            //Probabilistic decay
        }
        else if (chLeptonELossInstruction == 2)
        {
            cout << "Decaying charged lepton energy by CHLeptonDecayProbabilistic" << endl;
            handleChLepDecays = new EnergyLossDecay(
                &pythia.particleData,
                &pythia.rndm,
                energyLossDistributions::E_leptonic
            );

        }
        else
        {
            std::cerr << "chLeptonELossInstruction value not recognized" << std::endl;
            exit(1);
        }

        vector<int> handledChLeptons;
        for (auto it = chLeptons.begin(); it != chLeptons.end(); it++)
        {
            handledChLeptons.push_back((const int)*it);
            handledChLeptons.push_back(-(const int)*it);
        }
        mainDecayHandler->addHandler(handleChLepDecays, handledChLeptons);
    }
    pythia.setDecayPtr((DecayHandler*)mainDecayHandler, mainDecayHandler->getHandledParticles());
  
    pythia.init();

    cout << "Generating " << nEvent << " events of DM with mass " << dmMass << " GeV annihilating to " << partId << endl;
    if (showCS)  pythia.settings.listChanged();
    if (showCPD) pythia.particleData.listChanged();

    const unsigned int nBins = 300;

    //How many events were successfully generated
    TH1I *hEventStatus = new TH1I("eventStatus", "Event status distribution", 2, 0, 2);

    //neutrino fluxes per energy
    TH1D *hnuel = new TH1D("nuel", "Electron nu distribution", nBins, -9, 0);
    TH1D *hnumu = new TH1D("numu", "Muon nu distribution", nBins, -9, 0);
    TH1D *hnutau = new TH1D("nutau", "Tau nu distribution", nBins, -9, 0);

    //anti-neutrino fluxes per energy
    TH1D *hanuel = new TH1D("anuel", "Electron anti-nu distribution", nBins, -9, 0);
    TH1D *hanumu = new TH1D("anumu", "Muon anti-nu distribution", nBins, -9, 0);
    TH1D *hanutau = new TH1D("anutau", "Tau anti-nu distribution", nBins, -9, 0);

#ifdef NDEBUG
    TTree *tree = new TTree("events", "events");
    std::vector<float> *p_energy = new std::vector<float>();
    std::vector<int> *p_id = new std::vector<int>();
    std::vector<int> *p_parent = new std::vector<int>();
    std::vector<SolarNu::Particle> *p_particles = new std::vector<SolarNu::Particle>();
    TBranch *br1 = tree->Branch("particles_E", "std::vector<float>", &p_energy);
    TBranch *br2 = tree->Branch("particles_id", "std::vector<int>", &p_id);
    TBranch *br3 = tree->Branch("particles_parent", "std::vector<int>", &p_parent);
    //TBranch* br4 = tree->Branch("particles_cl", "std::vector<SolarNu::Particle>", &p_particles);

    //TBranch* br = tree->Branch("neutrinos", "std::vector<Particle>", &p_neutrinos);
    if (br1 == 0 || br2 == 0 || br3 == 0)
    {
        std::cerr << "Could not create branch, exiting" << std::endl;;
        exit(1);
    }
#endif


    int iAbort = 0;
    std::cout << "Starting event loop" << std::endl;

    TStopwatch *watch = new TStopwatch();
    watch->Start();
    for (int iEvent = 0; iEvent < nEvent; ++iEvent)
    {
#ifdef NDEBUG
        p_energy->clear();
        p_id->clear();
        p_parent->clear();
        p_particles->clear();
#endif

        // Generate events. Quit if many failures.
        if (!pythia.next())
        {
            if (++iAbort < nAbort)
            {
                hEventStatus->Fill(1);
                PythiaErrCount++;
                pythia.event.list();
                continue;
            }
            cerr << " Event generation aborted prematurely, owing to error!\n";
            break;
        }
        hEventStatus->Fill(0); //Fill with return code 0: successful event generation


        //Show how far along the run is
        if (nShow > 0 && iEvent % nShow == 0)
            cout << "Processed " << iEvent << " events" << endl;

        //List the events
        if (iEvent < nList)
        {
            pythia.info.list();
            pythia.event.list();
        }

        //decay neutrons
        //pythia.particleData.mayDecay(2112,true);
        //pythia.moreDecays();
        //pythia.particleData.mayDecay(2112,false);
        
        //Get spectra of final stable particles
        for (int i = 0; i < pythia.event.size(); ++i)
        {
            int id = pythia.event[i].id();
            int idAbs = abs(id);

            if (pythia.event[i].isFinal())
            {
                double x = log10((pythia.event[i].e() - pythia.event[i].m()) / dmMass);

                if (id == 12) hnuel->Fill(x);
                else if (id == -12) hanuel->Fill(x);

                else if (id == 14) hnumu->Fill(x);
                else if (id == -14) hanumu->Fill(x);

                else if (id == 16) hnutau->Fill(x);
                else if (id == -16) hanutau->Fill(x);



                //Put into event table
                if (idAbs == 12 || idAbs == 14 || idAbs == 16)
                {
#ifdef NDEBUG
                    p_energy->push_back(pythia.event[i].e());
                    p_id->push_back(pythia.event[i].id());
                    p_parent->push_back(pythia.event[pythia.event[i].mother1()].id());
#endif
                }
            }
        }
#ifdef NDEBUG
        tree->Fill();
#endif
    }
    pythia.statistics(true);
    watch->Stop();
    cout << "Processing took " << watch->RealTime() << " (" <<
         watch->CpuTime() << ") real(cpu) sec" << endl;

    cout << "Writing output from " << nEvent << " events." << endl;
    cout << "GSL error count: " << gslErrCount << endl;
    cout << "Pythia error count: " << PythiaErrCount << endl;
    cout << "Processing speed " << nEvent / watch->RealTime() << " events/sec" << endl;

    f.cd();
    f.Write();
    f.Close();

    return 0;
}
