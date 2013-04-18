import ROOT
import glob
import re
import sys
import pdb
import rootpy
import rootpy.io
import rootpy.plotting
import matplotlib.pyplot as plt
from collections import OrderedDict as dict
import numpy

import matplotlib
import matplotlib.cm
from numpy import *

from matplotlib import rc
rc('text', usetex=True)

lepNames = {"el": "Electron", "mu": "Muon", "tau": "Tau"}
lepPretty={"el":"#nu_{e}", "mu":"#nu_{#mu}", "tau":"#nu_{#tau}"}
flavour_names = {"nuel": r"$ \nu_e $", "numu": r"$ \nu_\mu $", "nutau": r"$ \nu_\tau $"}

decay_channel_names_Mathematica = {
    1: "q",
    2: "q",
    3: "q",
    4: "c",
    5: "b",
    6: "t",
    11: "e",
    13: "\\[Mu]",
    15: "\\[Tau]",
    #12: "\\[Nu]e",
    #14: "\\[Nu]\\[Mu]",
    #16: "\\[Nu]\\[Tau]",
    #22: "\\[Gamma]",
    #13: "mu",
    #15: "tau",

    12: "nue",
    14: "numu",
    16: "nutau",
    21: "g",
    22: "\\[Gamma]",

    23: "Z",
    24: "W",
    25: "h"
}

fstate_names_Mathematica = {
    #"deuterium": "d",
    #"electron": "e",
    #"proton": "p",
    #"neutron": "n",
    #"gamma": "\\[Gamma]",
    "nuel": "\\[Nu]e",
    "numu": "\\[Nu]\\[Mu]",
    "nutau": "\\[Nu]\\[Tau]"
}

energyLossMechanisms = {
        "no": "0",
        "chHadAvg": "100",
        "lHadAvg": "10",
        "chLepAvg": "1",
        "chHadProb": "200",
        "chLepProb": "2",
}

colorsELoss = {
        "no": ROOT.kBlack,
        "chHadAvg": ROOT.kRed,
        "lHadAvg": ROOT.kBlue,
        "chLepAvg": ROOT.kGreen,
        "chHadProb": ROOT.kYellow,
        "chLepProb": ROOT.kViolet,
}

colors = {1: ROOT.kBlue, 5: ROOT.kBlack, 6: ROOT.kRed, 22: ROOT.kOrange, 23: ROOT.kGreen, 24:ROOT.kTeal, 25: ROOT.kViolet, 11: ROOT.kPink+5, 12: ROOT.kViolet+5, 13: ROOT.kSpring+5, 15: ROOT.kBlue+5}
partNames = {1: r"$l\bar{l}$", 2:"l", 3:"l", 4: "l", 5: r"$c\bar{c}$", 6: r"$t\bar{t}$", 22: r"$\gamma\gamma$", 23: r"$Z_{0}$", 24: r"$W_{\pm}$", 25: r"$h_{0}$", 11:r"$e^{\pm}$", 13:r"$\mu^{\pm}$", 15:r"$\tau^{\pm}$", 12: r"$\nu_{e}$", }

hists = dict()

def cmap_discretize(cmap, N):
    """Return a discrete colormap from the continuous colormap cmap.

        cmap: colormap instance, eg. cm.jet.
        N: number of colors.

    Example
        x = resize(arange(100), (5,100))
        djet = cmap_discretize(cm.jet, 5)
        imshow(x, cmap=djet)
    """

    if type(cmap) == str:
        cmap = get_cmap(cmap)
    colors_i = concatenate((linspace(0, 1., N), (0.,0.,0.,0.)))
    colors_rgba = cmap(colors_i)
    indices = linspace(0, 1., N+1)
    cdict = {}
    for ki,key in enumerate(('red','green','blue')):
        cdict[key] = [ (indices[i], colors_rgba[i-1,ki], colors_rgba[i,ki]) for i in xrange(N+1) ]
    # Return colormap object.
    return matplotlib.colors.LinearSegmentedColormap(cmap.name + "_%d"%N, cdict, N)

class EnergyLoss:
    def __init__(self, *args):
        self.h_had = args[0]
        self.l_had = args[1]
        self.ch_lep = args[2]

    @staticmethod
    def lossName(loss):
        loss = int(loss)
        if loss == 0:
            return "no loss"
        elif loss == 1:
            return "avg. loss"
        elif loss==2:
            return "MC loss"
        else:
            raise ValueError("Loss {0} not recognized".format(loss))

    def bchad(self):
        return "b/c hadron: {0}".format(EnergyLoss.lossName(self.h_had))

    def lhad(self):
        return "light hadron: {0}".format(EnergyLoss.lossName(self.l_had))

    def chlep(self):
        return "charged lepton: {0}".format(EnergyLoss.lossName(self.ch_lep))

    def __str__(self):
        ret = ";\n".join([self.bchad(), self.lhad(), self.chlep()])
        return ret

class EnergyDistribution:
    def __init__(self, *args, **kwargs):
        self.hist = args[0]

        self.dm_mass = float(kwargs["dm_mass"])
        self.decay_channel = kwargs["decay_channel"]
        self.n_events = float(kwargs["n_events"])
        self.fstate = kwargs["fstate"]
        self.energy_loss = EnergyLoss(kwargs["h_had"], kwargs["l_had"], kwargs["ch_lep"])

        self.bin_width = 0.03
        if self.fstate in ["nuel", "numu", "nutau"]:
            mult = 0.5
        else:
            mult = 1.0
        sf = mult/(self.n_events*self.bin_width)
        pre_scale = self.hist.Integral()
        self.hist.Scale(sf)
        print "%s: pre-scale=%.3E, sf=%.3E, post-scale=%.3E" % (self, pre_scale, sf, self.hist.Integral())
        self.x = [x for x in self.hist.x()]
        self.y = [y for y in self.hist.y()]

    def printArray(self):
        decay_channel_str = decay_channel_names_Mathematica[int(self.decay_channel)]
        ostr = 'lxbins[\"{0}\"->\"{1}\",{2},Pythia]={{'.format(decay_channel_str, fstate_names_Mathematica[self.fstate], int(self.dm_mass))
        n_bins = self.hist.GetNbinsX()
        lxtab = numpy.linspace(-9, 0, 300)
        tot = sum([0.03*(10**lxtab[i - 1])*self.hist.GetBinContent(i) for i in range (1,301)])
        print "%s: %.2E" % (self, tot)
        for n in range(1, n_bins):
            ostr += ("%.9f" % float(self.hist.GetBinContent(n))) + ","
        ostr += ("%.9f" % (self.hist.GetBinContent(n_bins) + self.hist.GetBinContent(n_bins+1))) + "};"
        return ostr

    def __repr__(self):
        return "<EnergyDistribution(%s -> %s, %d)>" % (self.decay_channel, self.fstate, int(self.dm_mass))

    def __str__(self):
        return self.__repr__()

if __name__=="__main__":
    #f = rootpy.io.open("mergedOut/spec_Mar28.root")
    f = rootpy.io.open("spec_W_Apr18.root")

    pat = re.compile("mass_([0-9]*)/particle_([0-9]*)/energyLoss_hhad_([0-9])_lhad_([0-9])_chlep_([0-9])")
    interesting_hists = ["nuel", "numu", "nutau"]#{"nu_electron": "nuel", "nu_muon": "numu", "nu_tau": "nutau", "gamma": "gam", "proton": "proton", "neutron": "neutron", "ele":"el"}
    hists = dict()
    energy_distributions = dict()

    energy_distributions_part = dict()


    for elem in f.walk():
        m = pat.match(elem[0])
        if m is not None:
            mass = int(m.group(1))
            partId = int(m.group(2))
            hHadInstr = int(m.group(3))
            lHadInstr = int(m.group(4))
            chLepInstr = int(m.group(5))

            for hname in interesting_hists:
                name = elem[0] + "/" + hname
                hists[name] = f.get(name)
                event_status_hist = f.get(elem[0] + "/eventStatus")
                successful_events = float(event_status_hist.GetBinContent(1))
                if successful_events>0.0:
                    hists[name].Sumw2()
#                    hists[name].Scale(float(10**5)/float(successful_events))
                    energy_distributions[name] = EnergyDistribution(
                        hists[name],
                        dm_mass=mass,
                        decay_channel=partId,
                        h_had=hHadInstr,
                        l_had=lHadInstr,
                        ch_lep=chLepInstr,
                        n_events=successful_events,
                        fstate=hname
                    )
                    if hHadInstr==0 and lHadInstr==0 and chLepInstr==0:
                        if partId not in energy_distributions_part.keys():
                            energy_distributions_part[partId] = []
                        energy_distributions_part[partId].append(energy_distributions[name])
        else:
            continue

    channels = [1, 4, 5, 6, 11, 13, 15, 22, 23, 24, 25]
    for (part, distrs) in energy_distributions_part.items():
        if part not in channels:
            continue
        chan_name = decay_channel_names_Mathematica[part]
        #chan_name.replace("\\", r'\\\')
        ofname = "DM%s.nb" % chan_name
        ofile = open(ofname, "w")
        for dist in distrs:
            ofile.write(dist.printArray() + "\n")
        ofile.close()


    def drawHist(h, color, linestyle):
        bins = [x for x in h.x()]
        heights = [y for y in h.y()]
        errors = sqrt(heights)
        width = (max(bins) - min(bins))/float(len(bins))
        if numpy.sum(heights)>0:
            plt.errorbar(bins, heights, yerr=errors, alpha=0.8, color=color, fmt="r.", linestyle=linestyle, linewidth=2)

    def plot(s, fname, titleFormat=r"spectrum of E({neutrino_flavour}) for DM({mass})", legendFormat=r"{partname} {bc_had_loss}"):
        pat = re.compile(s)
        hns_to_plot = filter(lambda x: pat.match(x) is not None, hists.keys())
        plt.semilogy()

        #colors = cmap_discretize(matplotlib.cm.jet, len(hns_to_plot))
        colors=('k','y','m','c','b','g','r','#aaaaaa')
        linestyles=('-','--','-.',':', '.')
        styles=[(color,linestyle) for linestyle in linestyles for color in colors]

        i = 0
        for h in hns_to_plot:
            drawHist(hists[h], styles[i][0], styles[i][1])
            i += 1

        format_dicts = []
        for hn in hns_to_plot:
            d = dict()
            d["neutrino_flavour"] = flavour_names[hn.split("/")[-1]]
            d["bc_had_loss"] = energy_distributions[hn].energy_loss.bchad()
            d["l_had_loss"] = energy_distributions[hn].energy_loss.lhad()
            d["ch_lep_loss"] = energy_distributions[hn].energy_loss.chlep()
            d["partname"] = partNames[int(energy_distributions[hn].decay_channel)]
            d["mass"] = energy_distributions[hn].dm_mass

            print energy_distributions[hn].n_events
            format_dicts.append(d)
        leg_names = [legendFormat.format(**x) for x in format_dicts]
        plt.legend(leg_names, loc=2, fontsize='small', frameon=False)
        plt.xlim([-9, 0])
        plt.ylim([10, 10*max([max(distr.y) for distr in energy_distributions.values()])])
        plt.xlabel("E (GeV)")
        plt.ylabel("number of neutrinos / log energy bin / $10^5$ generated events")
        plt.title(titleFormat.format(**format_dicts[0]))

        ticks_log = linspace(-9, 0, 10)
        ticks_M = map(lambda x: (int(format_dicts[0]["mass"]))*10**x, ticks_log)
        plt.xticks(ticks_log, ticks_M)

        plt.savefig("nuSpec_%s.pdf" % fname)
        plt.clf()

    plot("mass_1000/particle_24/energyLoss_hhad_(.)*_lhad_(.)*_chlep_(.)*/numu", "W_numu",
            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating b/c hadron E loss", legendFormat=r"{neutrino_flavour} {bc_had_loss}")
#
#    plot(".*/particle_6/energyLoss_hhad_([0-9])_lhad_0_chlep_0/nutau", "top_hhad_nutau",
#            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating b/c hadron E loss", legendFormat=r"{neutrino_flavour} {bc_had_loss}")
#
#    plot(".*/particle_6/energyLoss_hhad_([0-9])_lhad_0_chlep_0/nuel", "top_hhad_nuel",
#            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating b/c hadron E loss", legendFormat=r"{neutrino_flavour} {bc_had_loss}")
#
#    plot(".*/particle_6/energyLoss_hhad_0_lhad_([0-9])_chlep_0/numu", "top_lhad_numu",
#            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating light hadron E loss", legendFormat=r"{neutrino_flavour}; {l_had_loss}")
#
#    plot(".*/particle_6/energyLoss_hhad_0_lhad_0_chlep_([0-9])/numu", "top_chlep_numu",
#            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating charged lepton E loss", legendFormat=r"{neutrino_flavour}; {ch_lep_loss}")
#
#    plot(".*/particle_15/energyLoss_hhad_([0-9])_lhad_0_chlep_0/numu", "tau_hhad_numu",
#            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating b/c hadron E loss", legendFormat=r"{neutrino_flavour}; {bc_had_loss}")
#
#    plot(".*/particle_15/energyLoss_hhad_0_lhad_([0-9])_chlep_0/numu", "tau_lhad_numu",
#            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating light hadron E loss", legendFormat=r"{neutrino_flavour}; {l_had_loss}")
#
#    plot(".*/particle_15/energyLoss_hhad_0_lhad_0_chlep_([0-9])/numu", "tau_chlep_numu",
#            titleFormat=r"spectrum of $E(\nu)$ for DM({mass}) $\rightarrow$ {partname}, variating charged lepton E loss", legendFormat=r"{neutrino_flavour}; {ch_lep_loss}")
#
#    plot(".*/particle_([0-9])+/energyLoss_hhad_0_lhad_0_chlep_0/numu", "all_no_loss", titleFormat=r"spectrum of E({neutrino_flavour}) for DM({mass}), no energy loss", legendFormat=r"{partname}")
#    plot("mass_1000/particle_([0-9])+/energyLoss_hhad_2_lhad_1_chlep_2/numu", "all_mc_loss", titleFormat=r"spectrum of E({neutrino_flavour}) for DM({mass}), MC energy loss", legendFormat=r"{partname}")
#    plot(".*/particle_([0-9])+/energyLoss_hhad_1_lhad_1_chlep_1/numu", "all_avg_loss", titleFormat=r"spectrum of E({neutrino_flavour}) for DM({mass}), average energy loss", legendFormat=r"{partname}")
#for (k, v) in energy_distributions.items():
