SHELL = /bin/sh
ARCH = Linux-gcc4
MYLCGPLATFORM = x86_64-slc5-gcc43-opt
SHAREDLIBS = no
FC = gfortran
CC = gcc
FFLAGS = -O2
#PYTHIA needs to be compiled with gcc~4.8 when using the -std=c++11 extension to avoid reference corruption on Event::entry vector resize due to implicit/buggy move constructor 
CFLAGS = -O3 -std=c++11 -W -Wall -Wextra -pedantic
CXXFLAGS = -O3 -std=c++11 -pedantic -W -Wall -Wshadow -Wextra
FLIBS = -lgfortran -lgfortranbegin
LDFLAGSSHARED = -O3 -pedantic -W -Wall -Wshadow -fPIC -shared
LDFLAGLIBNAME = -Wl,-soname
SHAREDSUFFIX = so
INSTALLDIR = .
DATADIR = .


#Aforementioned crash
#===========================================================
#There was a crash.
#This is the entire stack trace of all threads:
#===========================================================
##0  0x00007f0ad5a06f9e in waitpid () from /lib64/libc.so.6
##1  0x00007f0ad5999899 in do_system () from /lib64/libc.so.6
##2  0x00007f0adaa0fd48 in TUnixSystem::StackTrace() () from /home/software/root/lib/libCore.so
##3  0x00007f0adaa0e843 in TUnixSystem::DispatchSignals(ESignals) () from /home/software/root/lib/libCore.so
##4  <signal handler called>
##5  0x0000000000528708 in Pythia8::Particle::idAbs (this=0x7f0adb2c2030) at include/Event.h:148
##6  0x0000000000562169 in Pythia8::ParticleDecays::decay (this=0x7f0adb2fa4c8, iDec=1132, event=...) at src/ParticleDecays.cc:181
##7  0x0000000000522c7e in Pythia8::HadronLevel::next (this=0x7f0adb2f9da0, event=...) at src/HadronLevel.cc:193
##8  0x0000000000579c1e in Pythia8::Pythia::next (this=0x7f0adb2d5010) at src/Pythia.cc:1227
##9  0x000000000050bded in main (argc=8, argv=0x7fffa134baa8) at genSun.cc:728
#===========================================================
#
#
#The lines below might hint at the cause of the crash.
#If they do not help you then please submit a bug report at
#http://root.cern.ch/bugs. Please post the ENTIRE stack trace
#from above as an attachment in addition to anything else
#that might help us fixing this issue.
#===========================================================
##5  0x0000000000528708 in Pythia8::Particle::idAbs (this=0x7f0adb2c2030) at include/Event.h:148
##6  0x0000000000562169 in Pythia8::ParticleDecays::decay (this=0x7f0adb2fa4c8, iDec=1132, event=...) at src/ParticleDecays.cc:181
##7  0x0000000000522c7e in Pythia8::HadronLevel::next (this=0x7f0adb2f9da0, event=...) at src/HadronLevel.cc:193
##8  0x0000000000579c1e in Pythia8::Pythia::next (this=0x7f0adb2d5010) at src/Pythia.cc:1227
##9  0x000000000050bded in main (argc=8, argv=0x7fffa134baa8) at genSun.cc:728
#===========================================================
#
#
#(gdb) bt
##0  0x0000000000528708 in Pythia8::Particle::idAbs (this=0x7ffff7efe0c0) at include/Event.h:148
##1  0x0000000000562169 in Pythia8::ParticleDecays::decay (this=0x7ffff7f624c8, iDec=3154, event=...) at src/ParticleDecays.cc:181
##2  0x0000000000522c7e in Pythia8::HadronLevel::next (this=0x7ffff7f61da0, event=...) at src/HadronLevel.cc:193
##3  0x0000000000579c1e in Pythia8::Pythia::next (this=0x7ffff7f3d010) at src/Pythia.cc:1227
##4  0x000000000050bded in main (argc=8, argv=0x7fffffffe0d8) at genSun.cc:728
#(gdb) print decayer
#$12 = (Pythia8::Particle &) @0x7ffff7efe0c0: <error reading variable>
#(gdb) print event[iDec]
#$13 = (Pythia8::Particle &) @0x7ffff06d50c0: {static TINY = 9.9999999999999995e-21, idSave = 211, statusSave = -91, mother1Save = 3059, mother2Save = 0, daughter1Save = 3200, daughter2Save = 3201, colSave = 0, acolSave = 0, pSave = {static TINY = 9.9999999999999995e-21, 
#    xx = -0.7676885065095278, yy = 0.19574888782752364, zz = -0.10588933808552473, tt = -0.49436800040913187}, mSave = 0.13957, scaleSave = 0, polSave = 9, hasVertexSave = true, vProdSave = {static TINY = 9.9999999999999995e-21, xx = 6.9191560871311504, 
#    yy = -1.6171809685923042, zz = -2.0871202813005105, tt = 5.8985549327621944}, tauSave = 8762.3705182577487, pdePtr = 0xe23438, evtPtr = 0x7ffff7f3d078}
