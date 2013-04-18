#include <string.h>
#include <iostream>
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include <math.h>
#include <iomanip>

using namespace std;

void printArray(const char* ch, int m, TH1D* h) {
  cout << fixed;
  cout << "lxbins[\"" << ch << "\"->\"d\"," << m << ",Pythia]={";
  for (int i=1; i <= h->GetNbinsX()-1; i++)
    cout << h->GetBinContent(i) << ",";
  cout << h->GetBinContent(h->GetNbinsX()) << "};" << endl;
}

void printArray2(const char* ch, int m, TH1D* h, char *fs) {
  cout << fixed;
  cout << setprecision(9);
  cout << "lxbins[\"" << ch << "\"->\"" << fs << "\"," << m << ",Pythia]={";
  for (int i=1; i <= h->GetNbinsX()-1; i++)
    cout << h->GetBinContent(i) << ",";
  cout << h->GetBinContent(h->GetNbinsX())+h->GetBinContent(h->GetNbinsX()+1) << "};" << endl;
}

int main(int argc, char **argv) {
//  if (argc < 6) { 
//	printf("Usage: ./conv8 rootfile finalstate N NMeV shapeMeV\n");
//	exit(1);
//  }
  TFile f(argv[1]);
  //char * channel = argv[2];
  const char* channel = "e"; 
  float Nev = ((TH1I*)f.Get("mass_1000/particle_24/energyLoss_hhad_2_lhad_1_chlep_2/eventStatus"))->GetBinContent(1);
  std::cout << "N_ev = " << Nev << std::endl;
  //int MeV = atoi(argv[4]);
  //int MeV2 = atoi(argv[5]);
  int mass = 1000;
//  TH2D *d = (TH2D*)f.Get("antid");
//  TH1D *p = d->ProjectionX("p",0,MeV2);
//  TH1D *n = d->ProjectionX("n",0,MeV);
//  TH1D *pr = (TH1D*)f.Get("antip");
//  TH1D *e = (TH1D*)f.Get("el");
//  TH1D *g = (TH1D*)f.Get("gam");
//  TH1D *an = (TH1D*)f.Get("antin");
  TH1D *nue = (TH1D*)f.Get("mass_1000/particle_24/energyLoss_hhad_2_lhad_1_chlep_2/nuel");
  TH1D *num = (TH1D*)f.Get("mass_1000/particle_24/energyLoss_hhad_2_lhad_1_chlep_2/numu");
  TH1D *nut = (TH1D*)f.Get("mass_1000/particle_24/energyLoss_hhad_2_lhad_1_chlep_2/nutau");
  cout << "numu = " << num->Integral() << endl;
  // Use the shape of 400 MeV distribution, but the count of the 80 MeV one
 // double sc = double(n->Integral()) / double(p->Integral()) / (double(Nev) * 0.03);
 // p->Scale(sc);
 // pr->Scale(1./(Nev*0.03));
 // e->Scale(0.5/(Nev*0.03)); // Divided by 2 because it's e+ and e-, but we need only e+
 // g->Scale(1./(Nev*0.03));
  float sf = 0.5/(Nev*0.03);
  cout << "sf=" << sf << endl;
  nue->Scale(sf);
  num->Scale(sf);
  nut->Scale(sf);
  cout << "numu = " << num->Integral() << endl;
//  an->Scale(1./(Nev*0.03));
//  printArray(channel,mass,p);
//  printArray2(channel,mass,pr,"p");
//  printArray2(channel,mass,e,"e");
//  printArray2(channel,mass,g,"\\[Gamma]");
//  printArray2(channel,mass,an,"n");
  printArray2(channel,mass,nue,"\\[Nu]e");
  printArray2(channel,mass,num,"\\[Nu]\\[Mu]");
  printArray2(channel,mass,nut,"\\[Nu]\\[Tau]");
  return 0;
}
