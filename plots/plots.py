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
		#"no":		"000",
		#"chHadAvg":  "100",
		#"lHadAvg":   "010",
		#"chLepAvg":  "001",
		#"chHadProb": "200",
		"chLepProb": (0,0,2),
}
colorsELoss = {
		"no": ROOT.kBlack,
		"chHadAvg": ROOT.kRed,
		"lHadAvg": ROOT.kBlue,
		"chLepAvg": ROOT.kGreen,
		"chHadProb": ROOT.kYellow,
		"chLepProb": ROOT.kViolet,
}

colors = {
	1: ROOT.kBlue, 5: ROOT.kBlack, 6: ROOT.kRed,
	22: ROOT.kOrange, 23: ROOT.kGreen,24:ROOT.kTeal,
	25: ROOT.kViolet, 11: ROOT.kPink+5, 12: ROOT.kViolet+5,
	13: ROOT.kSpring+5, 15: ROOT.kBlue+5
}
partNames = {
	1: "l", 5:"c", 6:"t",
	22:"#gamma", 23:"Z_{0}", 24:"W_{#pm}",
	25: "h_{0}", 11:"e^{#pm}", 13:"#mu^{#pm}",
	15:"#tau^{#pm}", 12: "#nu_{e}"
}

f = ROOT.TFile("output.root")
rebin = 100
hists = dict()

def drawParticle(mass, pdgId, lepName):
	print 'Rebin:',rebin
	partStr = "particle_{0}".format(pdgId)
	title="Energy spectrum of {0} in channel DM({2})->2*{1}".format(lepPretty[lepName], partNames[pdgId], mass)
	histName = "nu{0}".format(lepName)
	massStr = "mass_{0}".format(mass)
	print massStr,partStr,histName
	for mechName,mech in energyLossMechanisms.items():
		mechStr = 'energyLoss_hhad_{0}_lhad_{1}_chlep_{2}'.format(*mech)
		print mechStr
		
		hists[mechName] = f.Get(massStr).Get(partStr).Get(mechStr).Get(histName)
		hists[mechName].Rebin(rebin)
		hists[mechName].SetLineColor(colorsELoss[mechName])
		if hists[mechName].Integral()==0.0:
			print 'Popping!'
			hists.pop(mechName)
			continue
		norm(hists[mechName])
#	j = 0
#	for h in [h1, h2, h3]:
#		h.GetXaxis().SetRangeUser(10, 1500)
#		h.SetLineColor(colors[pdgId]+j)
#		h.SetLineWidth(2)
#		norm(h)
#		h.SetStats(False)
#		h.SetTitle(title)
#		j += 1
#
	
	#c.SetLogy()
	firstDraw = True
	for hn, h in hists.items():
		print hn
		if firstDraw:
			h.Draw("H")
		else:
			h.Draw("H SAME")
		firstDraw = False
#	if firstDraw:
#		h1.Draw("H")
#	else:
#		h1.Draw("H SAME")
#	h2.Draw("H SAME")
#	h3.Draw("H SAME")
#	h2.SetLineStyle(2)
#	h3.SetLineStyle(9)
#	leg = ROOT.TLegend(0.75, 0.68, 0.88, 0.88)
#	leg.SetFillColor(ROOT.kWhite)
#	leg.SetLineColor(ROOT.kWhite)
#	leg.AddEntry(h1, "no")
#	leg.AddEntry(h2, "avg")
#	leg.AddEntry(h3, "MC")
#	leg.Draw("SAME")
	return hists

masses = [5000]
#particles = [1, 5, 6, 11, 13, 15, 22, 23, 24, 25]
particles = [25]
leps = ["el", "mu", "tau"]

def main():
	for m in masses:
		for p in particles:
			for l in leps:
				c = ROOT.TCanvas('cvs_{0}_p_{1}_l_{2}.png'.format(m, p, l))
				hists = drawParticle(m, p, l)
				#c.Print("out/nuE_m_{0}_p_{1}_l_{2}.png".format(m, p, l))
				c.SaveAs("out/nuE_m_{0}_p_{1}_l_{2}.png".format(m, p, l))

if __name__ == "__main__":
	sys.exit(main())
