genSun
======

Generates neutrinos in solar resonance decays using Pythia 8.

#Compiling

## Prerequisites 
* You need Git
* You need modern GCC (>=4.6 should be OK, tested using 4.7.2)
* You need to have ROOT installed and set up, check it by doing

>$ root-config --libs

The output should be something like
>$ root-config --libs
>-L/Applications/root/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lpthread -Wl,-rpath,/Applications/root/lib -lm -ldl

* Gnu Scientific Library, check it by doing

>$ ld -lgsl

The output must _NOT_ be
>$ ld -lgsl
>ld: library not found for -lgsl

##Getting the code
1. clone this repository by doing

>$ git clone git://github.com/HEP-KBFI/genSun.git

This will create a directory named 'genSun'

2. Download pythia and extract it into 'genSun/pythia8170'
3. Compile pythia
4. Go to the genSun code directory 'genSun/pythia8170/genSun'
5. compile the code using 'make -f Makefile-ied'

##Running the code
Run genSun.exe with the following parameters:
>$ genSun.exe PARTICLE MASS OUTPUTFILE.root INPUTFILE.card CHARGEDHADRONLOSS LIGHTHADRONLOSS CHARGEDLEPTONLOSS

* PARTICLE is a pdgId of a channel from the input card file to enable
* MASS is the mass of the resonance
* OUTPUTFILE.root is the filename of the output to make
* INPUTFILE.card is the input file you are using, e.g. 'cardSunTest.card'
* CHARGEDHADRONLOSS, LIGHTHADRONLOSS, CHARGEDLEPTONLOSS is a number form {0, 1, 2} specifying the type of energy loss mechanism to enable for this type of particle.
* The energy losses are: 0 - no energy loss (standard Pythia), 1 - average energy loss, 2 - probabilistic/MC energy loss

##Output
The output root file contains directories for each mass, loss mechanism and particle with the histograms nu{el, mu, tau} being the primary output.
In these histograms, the event-by-event distribution of log((E_nu - m_nu)/M_resonance) is plotted.
