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
