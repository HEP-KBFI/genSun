#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch()

histlist = ['nuel', 'numu', 'nutau', 'anuel', 'anumu', 'anutau']
physlist = {
	'energyLoss_hhad_0_lhad_0_chlep_0': ROOT.kRed,
	'physics_VAC_QGSP_BERT': ROOT.kBlue
}

tfile = ROOT.TFile('merged_mMay06_jApr28.root')
ofile = ROOT.TFile('output.root', 'RECREATE')

for key_mass in tfile.GetListOfKeys():
	mass = key_mass.GetTitle().split('_')[1]
	odir_mass = ofile.mkdir('mass_%s'%mass)
	odir_mass.cd()
	
	for key_particle in key_mass.ReadObj().GetListOfKeys():
		particle = int(key_particle.GetTitle().split('_')[1])
		
		avphys = [key_phys.GetTitle() for key_phys in key_particle.ReadObj().GetListOfKeys()]
		if not set(physlist).issubset(avphys):
			break
		print 'Particle: %i, Mass: %s'%(particle, mass)
		
		odir_particle = odir_mass.mkdir('particle_%i'%particle)
		odir_technical = odir_particle.mkdir('techical')
		
		for pname in physlist:
			pdir = key_particle.ReadObj().FindKey(pname).ReadObj()
			objs = [key.ReadObj() for key in pdir.GetListOfKeys()]
			pdir_new = odir_particle.mkdir(pdir.GetName())
			pdir_new.cd()
			for key in pdir.GetListOfKeys():
				key.ReadObj().Write()
		odir_particle.cd()
		
		subcvss = []
		
		for hname in histlist:
			def hname_f(hname, pname):
				print hname, pname
				if pname=='physics_VAC_QGSP_BERT':
					if hname=='nuel':
						return 'nue'
					elif hname=='anuel':
						return 'anue'
				return hname

			#hists = {pname: key_particle.ReadObj().FindKey(pname).ReadObj().FindKey(hname).ReadObj() for pname in physlist}
			hists = {pname: key_particle.ReadObj().FindKey(pname).ReadObj().FindKey(hname_f(hname, pname)).ReadObj() for pname in physlist}
			evstat = {pname: key_particle.ReadObj().FindKey(pname).ReadObj().FindKey('eventStatus').ReadObj().GetBinContent(1) for pname in physlist}
			
			cvs = ROOT.TCanvas(hname)
			
			ohstack = ROOT.THStack(hname + 'stack', 'Neutrino: ' + hname)
			legend = ROOT.TLegend(0.15, 0.65, 0.50, 0.85)
			
			for k,h in hists.iteritems():
				h.SetLineColor(physlist[k])
				h.Scale(1/evstat[k])
				
				ohstack.Add(h)
				legend.AddEntry(h,k)
			ohstack.Draw('nostack')
			legend.Draw()
			
			odir_technical.WriteObject(ohstack, ohstack.GetName())
			cvs.Write()
			
			#subcvss.append(cvs)
			subcvss.append(ohstack)
		
		sumcvs = ROOT.TCanvas('all')
		titletext = ROOT.TPaveText(0,0.97,1,1)
		titletext.SetFillColor(ROOT.kWhite); titletext.SetShadowColor(ROOT.kWhite); titletext.SetBorderSize(0)
		titletext.AddText('Particle %i @ %s [GeV]' % (particle, mass))
		titletext.Draw()
		
		sumcvs_pads = ROOT.TPad('add_pads_%s_%s'%(particle,mass), '', 0,0,1,0.95)
		sumcvs_pads.Divide(3,2)
		sumcvs_pads.Draw()
		
		i=1
		for cvs in subcvss:
			sumcvs_pads.cd(i)
			i=i+1
			cvs.Draw('nostack')
		sumcvs.Write()

tfile.Close()
ofile.Close()
