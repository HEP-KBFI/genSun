import ROOT
import glob
import re
import sys
import pdb
import rootpy
import rootpy.io
import rootpy.plotting

def norm(h):
    h.Sumw2()
    h.Scale(1.0/h.Integral())

lepNames = {"el": "Electron", "mu": "Muon", "tau": "Tau"}
lepPretty={"el":"#nu_{e}", "mu":"#nu_{#mu}", "tau":"#nu_{#tau}"}
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
partNames = {1: "l", 5:"c", 6:"t", 22:"#gamma", 23:"Z_{0}", 24:"W_{#pm}", 25: "h_{0}", 11:"e^{#pm}", 13:"#mu^{#pm}", 15:"#tau^{#pm}", 12: "#nu_{e}", }
f = ROOT.TFile("spec_Feb11.root")

rebin = 50
hists = dict()

"""
def drawParticle(mass, lepName, pdgId):

    partStr = "particle_{0}".format(pdgId)
    title="Energy spectrum of {0} in channel DM({2})->2*{1}".format(lepPretty[lepName], partNames[pdgId], mass)
    histName = "nu{0}".format(lepName)
    massStr = "mass_{0}".format(mass)
<<<<<<< HEAD
    for (mechName, mech) in energyLossMechanisms.items():
        hists[mechName] = f.Get(massStr).Get(partStr).Get("energyloss_{0}".format(mech)).Get(histName)
        hists[mechName].Rebin(rebin)
        hists[mechName].SetLineColor(colorsELoss[mechName])
        if hists[mechName].Integral()==0.0:
            hists.pop(mechName)
            continue
        norm(hists[mechName])
#    j = 0
#    for h in [h1, h2, h3]:
#        h.GetXaxis().SetRangeUser(10, 1500)
#        h.SetLineColor(colors[pdgId]+j)
#        h.SetLineWidth(2)
#        norm(h)
#        h.SetStats(False)
#        h.SetTitle(title)
#        j += 1
#
    c = ROOT.TCanvas()
    c.SetLogy()
    firstDraw = False
    for hn, h in hists.items():
        if firstDraw:
            h.Draw("H")
        else:
            h.Draw("H SAME")
        firstDraw = False
#    if firstDraw:
#        h1.Draw("H")
#    else:
#        h1.Draw("H SAME")
#    h2.Draw("H SAME")
#    h3.Draw("H SAME")
#    h2.SetLineStyle(2)
#    h3.SetLineStyle(9)
#    leg = ROOT.TLegend(0.75, 0.68, 0.88, 0.88)
#    leg.SetFillColor(ROOT.kWhite)
#    leg.SetLineColor(ROOT.kWhite)
#    leg.AddEntry(h1, "no")
#    leg.AddEntry(h2, "avg")
#    leg.AddEntry(h3, "MC")
#    leg.Draw("SAME")
    return c, hists
"""

"""
masses = [5000]
#particles = [1, 5, 6, 11, 13, 15, 22, 23, 24, 25]
particles = [15]
leps = ["el", "mu", "tau"]
"""

"""
for m in masses:
    for p in particles:
        for l in leps:
            c, hists = drawParticle(m, l, p)
            c.Print("plots/nuE_m_{0}_p_{1}_l_{2}.png".format(m, p, l))
"""

class EnergyLoss:
    def __init__(self, *args):
        self.h_had = args[0]
        self.l_had = args[1]
        self.ch_lep = args[2]

    def __str__(self):
        if self.h_had == 0 and self.l_had == 0 and self.ch_lep == 0:
            return "no E loss"
        elif self.h_had == 2 and self.l_had == 1 and self.ch_lep == 2:
            return "MC(bc had, ch. lep)"

class EnergyDistribution:
    def __init__(self, *args, *kwargs):
        hist = args[0]
        
        self.dm_mass = kwargs["dm_mass"]
        self.decay_channel = kwargs["decay_channel"]
        self.energy_loss = EnergyLoss(kwargs["h_had"], kwargs["l_had"], kwargs["ch_lep"])
        
        self.x = [x for x in hist.x()]
        self.y = [y for y in hist.y()]

if __name__=="__main__":
    f = rootpy.io.open("pythia8170/genSun/spec.root")

    pat = re.compile("mass_([0-9]*)/particle_([0-9]*)/energyLoss_hhad_([0-9])_lhad_([0-9])_chlep_([0-9])")
    interesting_hists = {"ele": "nuel", "mu": "numu", "tau":"nutau"}
    hists = dict()
    for elem in f.walk():
        m = pat.match(elem[0])
        if m is not None:
            mass = m.group(1)
            partId = m.group(2)
            hHadInstr = m.group(3)
            lHadInstr = m.group(4)
            chLepInstr = m.group(5)
            print mass, partId, hHadInstr, lHadInstr, chLepInstr
            
            
            for htitle, hname in interesting_hists.items():
                name = elem[0] + "/" + hname
                hists[name] = f.get(name)
                hists[name].rebin(250)
                
        else:
            continue

    def plot(s):
        pat = re.compile(s)
        hns_to_plot = filter(lambda x: pat.match(x) is not None, hists.keys())
        
        c = ROOT.TCanvas()
        fh = hists[hns_to_plot[0]]
        fh.Draw("E1")
        fh.GetYaxis().SetRangeUser(1, 10000000)
        for hn in hns_to_plot[1:]:
            hists[hn].Draw("SAME E1")
        c.SetLogy()


    plot(".*/particle_6/.*/nuel")

    
