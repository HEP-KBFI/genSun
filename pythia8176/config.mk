SHELL = /bin/sh
ARCH = Linux-gcc4
MYLCGPLATFORM = x86_64-slc5-gcc43-opt
SHAREDLIBS = no
FC = gfortran
CC = gcc
FFLAGS = -O2
CFLAGS = -O3 -std=c++11 -W -Wall -Wextra -pedantic
CXXFLAGS = -O3 -std=c++11 -pedantic -W -Wall -Wshadow -Wextra
FLIBS = -lgfortran -lgfortranbegin
LDFLAGSSHARED = -O3 -pedantic -W -Wall -Wshadow -fPIC -shared
LDFLAGLIBNAME = -Wl,-soname
SHAREDSUFFIX = so
INSTALLDIR = .
DATADIR = .
