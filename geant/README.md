Geant4 simulation
=================
Uses Geant4 to simulate the passage of particles through the sun
and then collect the neutrinos.


Build, compile and run
----------------------
Geant4 uses the cmake build system. You can use the `cmake` command
to create a build directory from the `geant/src` directory.
Note that you need to source `geant-install/bin/geant4.sh` first.
`makebuild.sh` creates the default build in `build/` directory
automatically.

After the cmake build has been configured, you should run `make` in
the `build/` directory. This will compile the code into an executable.
Note that cmake tracks changes to source files etc, so you do not have
to rerun the `makebuild.sh` all the time.


Usage
-----
The program takes a number of command line arguments.

	$ ./solnugeant --help
	Usage: solnugeant [OPTION...] <CHANNEL> <MASS>
	Simulate the annihilation of a dark matter particles in the Sun's core.

	It is assumed that the DM has a mass MASS and decays solely into particles
	specified by CHANNEL (which has to be the PDG ID of one of the particles in the
	created particle-antiparticle pair). The spectra of the created neutrinos will
	be stored to a ROOT file.

	 General options:
		  --bar                  use the antiparticle instead (i.e. the negative
								 PDG code); useful for single particle runs
		  --kinetic              use the provided energy as kinetic (particle gun
								 only).
	  -n, --runs=N               run the simulation N times
	  -o, --ofile=OFILE          write spectra to OFILE; default is output.root
	  -q, --quiet                reduce verbosity as much as possible
		  --seed=SEED            set the seed for the random generators; if this is
								 not specified, time(0) is used)
	  -u, --unit=UNIT            explicitly specify the unit of the <DM mass>;
								 supported options are 'G' for GeV and 'M' for MeV;
								 GeV is the default
	  -v, --vis                  open the GUI instead of running the simulation

	 Options for tweaking the physics:
	  -c, --creator=CREATOR      specify how the particles are created; possible
								 values are 'PYTHIA8' (default), 'P8single', 'G4'
								 or 'G4single'
	  -m, --material=MAT         specify the material of the world; possible values
								 are 'SUN' (default), 'SUNFULL', 'VAC', 'HIGHTEMP',
								 'IRONCORE'
	  -p, --physics=PHYSICS      specify the physics list; 'TRANS' refers to the
								 custom translation-only physics list; otherwise
								 the specified Geant4 reference physics list is
								 used; the default list is 'QGSP_BERT'
		  --radius=R             set the radius of the world in meters; the default
								 is 1000 meters; in vacuum the value is multiplied
								 by factor of 1 million
		  --short-neutron=on/off enable/disable short-lived neutrons
		  --track-kill=on/off    enable/disable the killing of low energy tracks
		  --track-verbose        print out the created Geant4 tracks

	 Other:
	  -?, --help                 Give this help list
		  --usage                Give a short usage message
	  -V, --version              Print program version

	Mandatory or optional arguments to long options are also mandatory or optional
	for any corresponding short options.


The option `-p` or `--physics` specifies the exact physics configuration
for the simulation. The values for `PHYSICS` can be either `TRANS`, which
refers to the custom translation-only physics list, or a name of a
[Geant4 reference physics list](http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/referencePL.shtml).
The default list is `QGSP_BERT`.

The option `-m` or `--material` specifies the exact material of the Sun.
The possible values for `MAT` and their meanings are given in the
[MATERIALS](/doc/MATERIALS.md) documentation file.

The option `-c` or `--creator` specifies the method which creates the
initial particles for the Geant4 simulation. The possible values for
`CREATOR` and their meanings are the following:

* `PYTHIA8` - uses PYTHIA8 to simulate the inital annihilation event and
              the decay of short-lived particles (and other effects).
              The stable and metastable particles are then inserted into
              the Geant4 simulation with a particle gun.
* `GEANT4`  - adds the initial particle-antiparticle pair directly to the
              the Geant4 simulation with a particle gun.
