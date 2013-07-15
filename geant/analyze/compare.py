#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch()

histlist = ['nuel', 'numu', 'nutau', 'anuel', 'anumu', 'anutau']
physlist = {
	#'energyLoss_hhad_0_lhad_0_chlep_0_noPiMinusAbs': ROOT.kRed,
	'energyLoss_hhad_2_lhad_1_chlep_2': ROOT.kGreen,
	#'physics_VAC_QGSP_BERT_P8': ROOT.kBlue,
	'physics_SUN_QGSP_BERT_trkON_P8': ROOT.kBlue+2
}

tfile = ROOT.TFile('fullinput.root')
ofile = ROOT.TFile('output.root', 'RECREATE')

for key_mass in tfile.GetListOfKeys():
	mass = key_mass.GetTitle().split('_')[1]
	mass_dir_done = False
	#print '> mass:', mass
	
	for key_particle in key_mass.ReadObj().GetListOfKeys():
		particle = int(key_particle.GetTitle().split('_')[1])
		#print '> > particle:', particle
		
		avphys = [key_phys.GetTitle() for key_phys in key_particle.ReadObj().GetListOfKeys()]
		if not set(physlist).issubset(avphys):
			continue
		print 'Particle: %i, Mass: %s'%(particle, mass)
		
		if not mass_dir_done:
			odir_mass = ofile.mkdir('mass_%s'%mass)
			odir_mass.cd()
			mass_dir_done = True
		odir_particle = odir_mass.mkdir('particle_%i'%particle)
		#odir_technical = odir_particle.mkdir('techical')
		
		def root_deepcopy_dir(dirname, src, dst):
			pdir = src.FindKey(dirname).ReadObj()
			ddir = dst.mkdir(dirname)
			for key in pdir.GetListOfKeys():
				obj = key.ReadObj()
				print '`%s` %s'%(type(obj), obj.GetName())
				if isinstance(obj, ROOT.TDirectory):
					root_deepcopy_dir(obj.GetName(), pdir, ddir)
				elif isinstance(obj, ROOT.TH1F) or isinstance(obj, ROOT.TH1D):
					obj.GetXaxis().SetTitle("log_{10}(E/m_{#chi})")
					ddir.WriteTObject(obj)
				else:
					ddir.WriteTObject(obj)
		
		for pname in physlist:
			root_deepcopy_dir(pname, key_particle.ReadObj(), odir_particle)
		odir_particle.cd()
		
		subcvss = []
		
		for hname in histlist:
			#hists = {pname: key_particle.ReadObj().FindKey(pname).ReadObj().FindKey(hname).ReadObj() for pname in physlist}
			#evstat = {pname: key_particle.ReadObj().FindKey(pname).ReadObj().FindKey('eventStatus').ReadObj().GetBinContent(1) for pname in physlist}
			hists = dict([(pname,key_particle.ReadObj().FindKey(pname).ReadObj().FindKey(hname).ReadObj()) for pname in physlist])
			evstat = dict([(pname, key_particle.ReadObj().FindKey(pname).ReadObj().FindKey('eventStatus').ReadObj().GetBinContent(1)) for pname in physlist])
			
			cvs = ROOT.TCanvas(hname)
			
			ohstack = ROOT.THStack(hname + 'stack', 'Neutrino: ' + hname)
			legend = ROOT.TLegend(0.15, 0.65, 0.50, 0.85)
			
			for k,h in hists.iteritems():
				if evstat[k] == 0:
					break
				h.SetLineColor(physlist[k])
				h.Scale(1/evstat[k])
				
				ohstack.Add(h)
				legend.AddEntry(h,k)
			ohstack.Draw('nostack')
			legend.Draw()
			
			ohstack.GetXaxis().SetTitle("log_{10}(E/m_{#chi})")
			ohstack.GetYaxis().SetTitle("N_{#nu} per event")
			
			#odir_technical.WriteObject(ohstack, ohstack.GetName())
			cvs.SetLogy()
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
			ROOT.gPad.SetLogy()
			cvs.Draw('nostack')
		sumcvs.Write()

tfile.Close()
ofile.Close()
