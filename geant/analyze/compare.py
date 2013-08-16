#!/usr/bin/env python
import ROOT
ROOT.gROOT.SetBatch()

import operator

histlist = ['nuel', 'numu', 'nutau', 'anuel', 'anumu', 'anutau']
physlist = {
	#'energyLoss_hhad_0_lhad_0_chlep_0_noPiMinusAbs': ROOT.kRed,
	'energyLoss_hhad_2_lhad_1_chlep_2': ROOT.kGreen,
	#'physics_VAC_QGSP_BERT_P8': ROOT.kBlue,
	'physics_SUN_QGSP_BERT_trkON_P8': ROOT.kBlue+2
}

def root_deepcopy_dir(dirname, src, dst):
	"""Perform a deep copy of a ROOT directory."""
	pdir = src.FindKey(dirname).ReadObj()
	ddir = dst.mkdir(dirname)
	for key in pdir.GetListOfKeys():
		obj = key.ReadObj()
		print '`%s` %s'%(type(obj), obj.GetName())
		if isinstance(obj, ROOT.TDirectory):
			root_deepcopy_dir(obj.GetName(), pdir, ddir)
#		elif isinstance(obj, ROOT.TH1F) or isinstance(obj, ROOT.TH1D):
#			obj.GetXaxis().SetTitle("log_{10}(E/m_{#chi})")
#			ddir.WriteTObject(obj)
		else:
			ddir.WriteTObject(obj)

def find_physics(tfile):
	"""Finds all the physics settings from a TFile.

	Takes an input TFile (any TDirectory would do actually) and returns
	a list of tuples with physics settings. The tuple elements are mass,
	pid and a list of all corresponding physics settings.

	"""
	ret = []
	for key_mass in tfile.GetListOfKeys():
		mass = key_mass.GetTitle().split('_')[1]
		for key_particle in key_mass.ReadObj().GetListOfKeys():
			particle = int(key_particle.GetTitle().split('_')[1])
			val = [key_phys.GetName() for key_phys in key_particle.ReadObj().GetListOfKeys()]
			ret.append((mass,particle,val))
	return ret

def filter_physics(t):
	phys_in_val = map(lambda s: s in t[2], physlist)
	has_all = reduce(operator.and_, phys_in_val)
	return has_all

class NeutrinoStack:
	def __init__(self, hname):
		self._thstack = ROOT.THStack(hname + 'stack', 'Neutrino: ' + hname)
		self._legend = ROOT.TLegend(0.15, 0.65, 0.50, 0.85)
		self._hs = []
	def add(self, h):
		self._hs.append(h)
		self._thstack.Add(h)
		self._legend.AddEntry(h,h.GetTitle())
	def Draw(self):
		self._thstack.Draw('nostack')
		self._thstack.GetXaxis().SetTitle('log_{10}(E/m_{#chi})')
		self._thstack.GetYaxis().SetTitle('N_{#nu} per event')
		self._legend.Draw()

def stack_nu_hists(hs, title='NuStack'):
	stack = NeutrinoStack(title)
	for h in hs:
		stack.add(h)
	return stack

def write_nu_stacks(nu_stacks, name='', title=''):
	subcvss = []
	for hname,stack in nu_stacks.iteritems():
		cvs = ROOT.TCanvas(hname)
		stack.Draw()
		cvs.SetLogy()
		cvs.Write()
		subcvss.append(cvs)
	# Write the merged canvas
	sumcvs = ROOT.TCanvas('all')
	titletext = ROOT.TPaveText(0,0.97,1,1)
	titletext.SetFillColor(ROOT.kWhite); titletext.SetShadowColor(ROOT.kWhite); titletext.SetBorderSize(0)
	titletext.AddText(title)
	titletext.Draw()

	sumcvs_pads = ROOT.TPad(name, '', 0,0,1,0.95)
	sumcvs_pads.Divide(3,2)
	sumcvs_pads.Draw()

	i=1
	for hname,stack in nu_stacks.iteritems():
		sumcvs_pads.cd(i)
		i=i+1
		ROOT.gPad.SetLogy()
		stack.Draw()
	sumcvs.Write()

if __name__ == '__main__':
	tfile = ROOT.TFile('fullinput.root')
	ofile = ROOT.TFile('output.root', 'RECREATE')

	physics = find_physics(tfile)
	physics = filter(filter_physics, physics)
	physics.sort()

	masses = set(map(lambda t: t[0], physics))
	for m in masses:
		print 'Mass:', m
		odir_mass = ofile.mkdir('mass_{0}'.format(m))
		pids = set(map(lambda t: t[1], filter(lambda t: t[0]==m, physics)))
		for pid in pids:
			odir_pid = odir_mass.mkdir('particle_{0}'.format(pid))
			idir = tfile.Get('mass_{0}/particle_{1}'.format(m,pid))
			for p in physlist:
				root_deepcopy_dir(p, idir, odir_pid)
			nu_stacks = {}
			for hname in histlist:
				hs = []
				for p in physlist:
					evstat = idir.Get('{0}/{1}'.format(p,'eventStatus')).GetBinContent(1)
					h = idir.Get('{0}/{1}'.format(p,hname))
					h.SetTitle(p)
					h.SetLineColor(physlist[p])
					h.Scale(1/evstat)
					hs.append(h)
				nu_stacks[hname] = stack_nu_hists(hs, title=hname)
			# Write stuff
			odir_pid.cd()
			write_nu_stacks(nu_stacks, name='add_pads_%s_%s'%(pid,m), title='Particle %i @ %s [GeV]' % (pid, m))
	ofile.Close()
	tfile.Close()
