# Setup for gSOAP server version
# Simeon Warner, 2007-02...
# $Id: Makefile,v 1.9 2011-03-10 04:59:44 simeon Exp $
#
# Edit this to the location of your gsoap installation:

# RHEL6 with gsoap and gsoap-devel 2.7.16 as of 2014-07
# `sudo yum install gsoap gsoap-devel`
# Change GSOAP_PREFIX if installed elsewhere
GSOAP_PREFIX=/usr

GSOAP_BIN=$(GSOAP_PREFIX)/bin
GSOAP_INC=$(GSOAP_PREFIX)/include

# Used only for overlapd-static
GSOAP_SRC=gsoap

################## Makefile should not need configuration below this line ########################

WSDL2H=$(GSOAP_BIN)/wsdl2h
SOAPCPP2=$(GSOAP_BIN)/soapcpp2
STDSOAP2H=$(GSOAP_INC)/stdsoap2.h

# DocSim libs
#
DSOBJ= ../lib/DocSet.o ../lib/DocInfo.o ../lib/Logger.o ../lib/KgramInfo.o ../lib/KeyMap.o ../lib/MarkedDoc.o ../lib/KeyTable.o ../lib/KeyTable3Element.o ../lib/DocPair.o ../lib/kgrams.o ../lib/files.o ../lib/options.o ../lib/pstats.o ../lib/anystream.o ../include/gzstream.o

# Compiler settings
#
CPP=g++
LIBS=
COFLAGS=-g -O
CDFLAGS=$(GLOBAL_CPPDEFS)
CWFLAGS=-Wall
CIFLAGS=-I../.. -lz
CMFLAGS=
CFLAGS= $(CWFLAGS) $(COFLAGS) $(CDFLAGS) $(CIFLAGS) $(CMFLAGS)

.PHONY: all
all:	overlapd

overlap.h: overlap.wsdl typemap.dat
	$(WSDL2H) -s -t typemap.dat overlap.wsdl

soapServer.cpp: overlap.h
	$(SOAPCPP2) overlap.h

# Normal binary uses gsoap++ shared system libraries 
overlapd: overlapd.cpp soapServer.cpp $(DSOBJ)
	$(CPP) $(CFLAGS) -l gsoap++ -I ../lib -o overlapd overlapd.cpp soapC.cpp soapServer.cpp $(STDSOAP2H) $(DSOBJ) $(LIBS)

# Alternative using compiled in gsoap (must match versions of wsdl2h & soapcpp2)
overlapd-static: overlapd.cpp soapServer.cpp $(DSOBJ)
	$(CPP) $(CFLAGS) -I gsoap -I ../lib -o overlapd-static overlapd.cpp soapC.cpp soapServer.cpp $(GSOAP_SRC)/stdsoap2.cpp $(DSOBJ) $(LIBS)

.PHONY: clean
clean:
	rm -f *.o soapH.h soapStub.h soapC.cpp soapC.c soapClient.cpp soapClient.c soapServer.cpp soapServer.c soapClientLib.cpp soapServerLib.cpp soap*Proxy.h soap*Object.h overlap.h overlapd overlapd-static

.PHONY: distclean
distclean: clean
	rm -f *.xml *.nsmap *.log quote
