import ROOT
import glob
import re
import sys
import pdb

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

masses = [5000]
#particles = [1, 5, 6, 11, 13, 15, 22, 23, 24, 25]
particles = [15]
leps = ["el", "mu", "tau"]

for m in masses:
    for p in particles:
        for l in leps:
            c, hists = drawParticle(m, l, p)
            c.Print("plots/nuE_m_{0}_p_{1}_l_{2}.png".format(m, p, l))

