SHELL = /bin/sh

INCDIR=include
SRCDIR=src
LIBDIR=lib
LIBDIRARCH=lib/archive
PYTHIA8LOCATION=..
CC=g++
ROOTFLAGS=`root-config --cflags --ldflags --libs`
CFLAGS= -O2 -ansi -pedantic -W -Wall -Wshadow -I$(PYTHIA8LOCATION)/$(INCDIR)
LDFLAGS= -L$(PYTHIA8LOCATION)/$(LIBDIRARCH) -O2 -ansi -pedantic -W -Wall -Wshadow -lpythia8 -llhapdfdummy

genSun: genSun.cc
	$(CC) $(CFLAGS) $(ROOTFLAGS) $@.cc -o $@.exe $(LDFLAGS)
