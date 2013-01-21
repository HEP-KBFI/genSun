SHELL = /bin/sh

INCDIR=include
SRCDIR=src
LIBDIR=lib
LIBDIRARCH=lib/archive
PYTHIA8LOCATION=/Users/joosep/Documents/pythia8170
#CC=g++
CC=clang++
ROOTFLAGS=`root-config --cflags --ldflags --libs`
CFLAGS= -std=c++11 -stdlib=libc++ $(ROOTFLAGS) -funsigned-char -mmacosx-version-min=10.8 -keep_private_externs -arch x86_64 -O2 -pedantic -W -Wall -Wshadow -Wlong-long -Wunused-variable -I$(PYTHIA8LOCATION)/$(INCDIR) -I/Users/joosep/Documents/gsl
LDFLAGS= -L$(PYTHIA8LOCATION)/$(LIBDIRARCH) -L/Users/joosep/Documents/gsl/libs -O2 -pedantic -W -Wall -Wshadow -flat_namespace -lpythia8 -llhapdfdummy -lgsl
genSun: genSun.cc
	$(CC) $(CFLAGS) $@.cc $(LDFLAGS) $(FILELIST) -v -o $@.exe
