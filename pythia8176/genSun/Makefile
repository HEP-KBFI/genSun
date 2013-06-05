SHELL = /bin/sh

INCDIR=include
SRCDIR=src
LIBDIR=lib
LIBDIRARCH=lib/archive
PYTHIA8LOCATION=..
#CC=g++
CC=/usr/bin/clang++ -v
ROOTFLAGS=`root-config --cflags --ldflags --libs`
CFLAGS= -std=c++11 -stdlib=libc++ $(ROOTFLAGS) -funsigned-char -keep_private_externs -arch x86_64 -O3 -pedantic -W -Wall -I$(PYTHIA8LOCATION)/$(INCDIR)
LDFLAGS= -L$(PYTHIA8LOCATION)/$(LIBDIRARCH) -O3 -pedantic -W -Wall -Wshadow -flat_namespace -lpythia8 -llhapdfdummy -lgsl
genSun: genSun.cc
	$(CC) $(CFLAGS) $@.cc $(LDFLAGS) $(FILELIST) -v -o $@.exe
