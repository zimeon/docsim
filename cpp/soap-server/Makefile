# Setup for gSOAP server version
# Simeon Warner, 2007-02...
# $Id: Makefile,v 1.8 2011-02-28 23:49:53 simeon Exp $
#
# Edit this to the location of your gsoap installation:

GSOAP_PREFIX=/opt/gsoap-2.7.12
GSOAP_SRC=gsoap

################## Makefile should not need configuration below this line ########################

WSDL2H=$(GSOAP_PREFIX)/bin/wsdl2h
GSOAP=$(GSOAP_PREFIX)/bin/soapcpp2
SOAPH=$(GSOAP_SRC)/stdsoap2.h
SOAPC=$(GSOAP_SRC)/stdsoap2.c
SOAPC=$(GSOAP_SRC)/stdsoap2.cpp

# DocSim libs
#
DSOBJ= ../lib/DocSet.o ../lib/DocInfo.o ../lib/Logger.o ../lib/KgramInfo.o ../lib/KeyMap.o ../lib/MarkedDoc.o ../lib/KeyTable.o ../lib/KeyTable3Element.o ../lib/DocPair.o ../lib/kgrams.o ../lib/files.o ../lib/options.o ../lib/pstats.o

# Compiler settings
#
CPP=g++
LIBS=
COFLAGS=-O
CWFLAGS=-Wall
CIFLAGS=-I../..
CMFLAGS=
CFLAGS= $(CWFLAGS) $(COFLAGS) $(CIFLAGS) $(CMFLAGS)

all:	overlapd

overlap.h: overlap.wsdl typemap.dat
	$(WSDL2H) -s overlap.wsdl

soapServer.cpp: overlap.h
	$(GSOAP) overlap.h

overlapd: overlapd.cpp soapServer.cpp $(DSOBJ)
	$(CPP) $(CFLAGS) -I gsoap -I ../lib -o overlapd overlapd.cpp soapC.cpp soapServer.cpp $(SOAPC) $(DSOBJ) $(LIBS)

clean:
		rm -f *.o soapH.h soapStub.h soapC.cpp soapC.c soapClient.cpp soapClient.c soapServer.cpp soapServer.c soap*Proxy.h overlapd

distclean:
		rm -f *.o *.xml *.nsmap *.log soapH.h soapStub.h soapC.cpp soapC.c soapClient.cpp soapClient.c soapServer.cpp soapServer.c soap*Proxy.h quote overlapd
