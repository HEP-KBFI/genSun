SHELL = /bin/sh

INCDIR=include
SRCDIR=src
LIBDIR=lib
LIBDIRARCH=lib/archive
PYTHIA8LOCATION=..
#CC=g++
CC=clang++
ROOTFLAGS=`root-config --cflags --ldflags --libs`
CFLAGS= -stdlib=libstdc++ -funsigned-char -mmacosx-version-min=10.8 -keep_private_externs -arch x86_64 -O2 -pedantic -W -Wall -Wshadow -Wlong-long -Wunused-variable -I$(PYTHIA8LOCATION)/$(INCDIR)
LDFLAGS= -L$(PYTHIA8LOCATION)/$(LIBDIRARCH) -O2 -pedantic -W -Wall -Wshadow -flat_namespace -lpythia8 -llhapdfdummy
genSun: genSun.cc
	$(CC) $(CFLAGS) $(ROOTFLAGS) $@.cc $(LDFLAGS) $(FILELIST) -v -o $@.exe
