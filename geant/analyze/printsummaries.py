#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('fname', type=str)
args = parser.parse_args()
#print 'Arguments:', args

nus = ['nuel', 'anuel', 'numu', 'anumu', 'nutau', 'anutau']

import ROOT
ROOT.gROOT.SetBatch()

nufmt = ' '.join(map(lambda n: '{%d:15,.0f}'%n, range(2,8)))
nustrfmt = ' '.join(map(lambda n: '{%d:>15s}'%n, range(0,6)))
#print nufmt,nustrfmt

pnames = {
    1: 'down', 2: 'up', 3: 's', 4: 'c', 5: 'b', 6: 't',
	11: 'e', 13: 'mu', 15: 'tau',
	12: 'nue', 14: 'numu', 16: 'nutau',
	21: 'g', 22: 'gamma', 23: 'Z', 24: 'W', 25: 'h',
	321: 'K+'
}

def decode_mass(mstr):
	if mstr[-1] == 'G':
		return float(mstr[0:-1])
	elif mstr[-1] == 'M':
		return float(mstr[0:-1])/1.0e3
	else:
		return float(mstr)

tfile = ROOT.TFile(args.fname)
for mass in sorted(map(lambda k: (k.GetName().split('_')[1]), tfile.GetListOfKeys())):
	key_mass = tfile.FindKey('mass_{0}'.format(mass))
	print
	print 'DM particle mass: {0:11,.6f} GeV'.format(decode_mass(mass))
	print 'channel     events '+nustrfmt.format(*nus)
	for pid in sorted(map(lambda k: int(k.GetName().split('_')[1]), key_mass.ReadObj().GetListOfKeys())):
		key_particle = key_mass.ReadObj().FindKey('particle_{0}'.format(pid))
		for key_physics in key_particle.ReadObj().GetListOfKeys():
			physObj = key_physics.ReadObj()
			pid = int(key_particle.GetName().split('_')[1])
			evStatHisto = physObj.GetKey('eventStatus').ReadObj()
			if evStatHisto:
				#print key_mass.GetName(),key_particle.GetName(),key_physics.GetName()
				events = evStatHisto.GetBinContent(1)
				nucounts = dict(map(lambda n: (n, physObj.GetKey(n).ReadObj().Integral()), nus))
				print ('{0:7s} {1:10,d} '+nufmt).format(pnames[pid], int(events), nucounts['nuel'], nucounts['anuel'], nucounts['numu'], nucounts['anumu'], nucounts['nutau'], nucounts['anutau'])
			else:
				print '!! BAD !!', key_mass.GetName(),key_particle.GetName(),key_physics.GetName()
tfile.Close()
