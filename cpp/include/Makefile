# Makefile for imported C++ code needed by DocSim suite (./include)
# Simeon Warner - 2011

CPP = gcc
CPPDEFS = 
CPPFLAGS = -g -O -Wall -I . -I lib

.cpp.o:
	$(CPP) $(CPPDEFS) $(CPPFLAGS) -c $<

all: gzstream.o

gzstream.o: gzstream/gzstream.o
	cd gzstream && make gzstream.o && cp gzstream.o ..

.PHONY: clean
clean:
	rm -f gzstream.o
	cd gzstream && make clean
