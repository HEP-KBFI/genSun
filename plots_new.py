import ROOT
import glob
import re
import sys
import pdb
import rootpy
import rootpy.io
import rootpy.plotting
import matplotlib.pyplot as plt

import matplotlib
import matplotlib.cm
from numpy import *

from matplotlib import rc
rc('text', usetex=True)

def norm(h):
    h.Sumw2()
    h.Scale(1.0/h.Integral())

lepNames = {"el": "Electron", "mu": "Muon", "tau": "Tau"}
lepPretty={"el":"#nu_{e}", "mu":"#nu_{#mu}", "tau":"#nu_{#tau}"}
flavour_names = {"nuel": r"$ \nu_e $", "numu": r"$ \nu_\mu $", "nutau": r"$ \nu_\tau $"}

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
partNames = {1: r"l\bar{l}", 5: r"c\bar{c}", 6: r"t\bar{t}", 22: r"\gamma\gamma", 23: r"Z_{0}", 24: r"W_{\pm}", 25: r"h_{0}", 11:r"e^{\pm}", 13:r"\mu^{\pm}", 15:r"\tau^{\pm}", 12: r"\nu_{e}", }

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

    def __str__(self):
        ret = "b/c hadron: {0}".format(EnergyLoss.lossName(self.h_had))
        ret += "; light hadron: {0}".format(EnergyLoss.lossName(self.l_had))
        ret += "; charged lepton: {0}".format(EnergyLoss.lossName(self.ch_lep))
        return ret

class EnergyDistribution:
    def __init__(self, *args, **kwargs):
        hist = args[0]

        self.dm_mass = float(kwargs["dm_mass"])
        self.decay_channel = kwargs["decay_channel"]
        self.energy_loss = EnergyLoss(kwargs["h_had"], kwargs["l_had"], kwargs["ch_lep"])

        self.x = [x for x in hist.x()]
        self.y = [y for y in hist.y()]

if __name__=="__main__":
    f = rootpy.io.open("spec_Feb26.root")

    pat = re.compile("mass_([0-9]*)/particle_([0-9]*)/energyLoss_hhad_([0-9])_lhad_([0-9])_chlep_([0-9])")
    interesting_hists = {"ele": "nuel", "mu": "numu", "tau":"nutau"}
    hists = dict()
    energy_distributions = dict()

    for elem in f.walk():
        m = pat.match(elem[0])
        print elem[0]
        if m is not None:
            mass = m.group(1)
            partId = m.group(2)
            hHadInstr = m.group(3)
            lHadInstr = m.group(4)
            chLepInstr = m.group(5)

            for htitle, hname in interesting_hists.items():
                name = elem[0] + "/" + hname
                hists[name] = f.get(name)
                hists[name].Sumw2()
                hists[name].rebin(250)
                energy_distributions[name] = EnergyDistribution(hists[name], dm_mass=mass, decay_channel=partId, h_had=hHadInstr, l_had=lHadInstr, ch_lep=chLepInstr)

        else:
            continue
    def drawHist(h, color="r"):
        bins = [x for x in h.x()]
        heights = [y for y in h.y()]
        errors = sqrt(heights)
        width = (max(bins) - min(bins))/float(len(bins))
        plt.errorbar(bins, heights, yerr=errors, alpha=0.5, color=color, fmt="r.", linestyle="-", linewidth=3)

    def plot(s, fname, title):
        pat = re.compile(s)
        print hists
        hns_to_plot = filter(lambda x: pat.match(x) is not None, hists.keys())
        print hns_to_plot
        plt.semilogy()

        i = 0
        colors = cmap_discretize(matplotlib.cm.jet, len(hns_to_plot))
        for h in hns_to_plot:
            drawHist(hists[h], color=colors(i))
            i += 1

        neutrino_flavours = [flavour_names[x.split("/")[-1]] for x in hns_to_plot]
        leg_names = [str(energy_distributions[hn].energy_loss) for hn in hns_to_plot]
        for i in range(len(leg_names)):
            leg_names[i] += " " + neutrino_flavours[i]
        print leg_names
        plt.legend(leg_names, loc=4, fontsize='small', frameon=False)
        plt.xlim([-9, 0])
        plt.ylim([10, 10*max([max(distr.y) for distr in energy_distributions.values()])])
        plt.xlabel("E (GeV)")
        plt.title(r"$E_\nu$ from $ DM(%.0f) \rightarrow %s $, %s" % (energy_distributions[hns_to_plot[0]].dm_mass , partNames[int(energy_distributions[hns_to_plot[0]].decay_channel)], title))

        ticks_log = linspace(-9, 0, 10)
        ticks_M = map(lambda x: 5000*10**x, ticks_log)
        plt.xticks(ticks_log, ticks_M)

        plt.savefig("nuSpec_%s.pdf" % fname)
        plt.clf()

    #plot(".*/particle_6/energyLoss_hhad_0_lhad_0_chlep_([0-9])/numu")
    #plot(".*/particle_15/energyLoss_hhad_0_lhad_([0-9])_chlep_0/numu")
    plot(".*/particle_6/energyLoss_hhad_([0-9])_lhad_0_chlep_0/numu", "top_hhad_numu", "variating b/c hadron loss")
    plot(".*/particle_6/energyLoss_hhad_([0-9])_lhad_0_chlep_0/numu", "top_hhad_nuel", "variating b/c hadron loss")

    plot(".*/particle_6/energyLoss_hhad_0_lhad_([0-9])_chlep_0/numu", "top_lhad_numu", "variating light hadron loss")
    plot(".*/particle_6/energyLoss_hhad_0_lhad_([0-9])_chlep_0/numu", "top_lhad_nuel", "variating light hadron loss")

    plot(".*/particle_6/energyLoss_hhad_0_lhad_0_chlep_([0-9])/numu", "top_chlep_numu", "variating charged lepton loss")
    plot(".*/particle_6/energyLoss_hhad_0_lhad_0_chlep_([0-9])/numu", "top_chlep_nuel", "variating charged lepton loss")

    plot(".*/particle_15/energyLoss_hhad_([0-9])_lhad_0_chlep_0/numu", "tau_hhad_numu", "variating b/c hadron loss")
    plot(".*/particle_15/energyLoss_hhad_([0-9])_lhad_0_chlep_0/numu", "tau_hhad_nuel", "variating b/c hadron loss")

    plot(".*/particle_15/energyLoss_hhad_0_lhad_([0-9])_chlep_0/numu", "tau_lhad_numu", "variating light hadron loss")
    plot(".*/particle_15/energyLoss_hhad_0_lhad_([0-9])_chlep_0/numu", "tau_lhad_nuel", "variating light hadron loss")

    plot(".*/particle_15/energyLoss_hhad_0_lhad_0_chlep_([0-9])/numu", "tau_chlep_numu", "variating charged lepton loss")
    plot(".*/particle_15/energyLoss_hhad_0_lhad_0_chlep_([0-9])/numu", "tau_chlep_nuel", "variating charged lepton loss")



