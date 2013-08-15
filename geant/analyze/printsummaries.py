#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('fname', type=str)
args = parser.parse_args()
print 'Arguments:', args

nus = ['nuel', 'anuel', 'numu', 'anumu', 'nutau', 'anutau']

import ROOT
ROOT.gROOT.SetBatch()

tfile = ROOT.TFile(args.fname)
for key_mass in tfile.GetListOfKeys():
	for key_particle in key_mass.ReadObj().GetListOfKeys():
		for key_physics in key_particle.ReadObj().GetListOfKeys():
			physObj = key_physics.ReadObj()
			
			evStatHisto = physObj.GetKey('eventStatus').ReadObj()
			if evStatHisto:
				print key_mass.GetName(),key_particle.GetName(),key_physics.GetName()
			else:
				print '!! BAD !!', key_mass.GetName(),key_particle.GetName(),key_physics.GetName()
			print ', '.join(map(lambda n: '{0}:{1}'.format(n, bool(physObj.GetKey(n).ReadObj())), nus))
			#print physObj.GetKey('eventStatus'), physObj.GetKey('eventStatus').ReadObj(), bool(physObj.GetKey('eventStatus').ReadObj())
tfile.Close()
