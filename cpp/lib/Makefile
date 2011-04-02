# Sentence used only by SimSearch2
# SimSearch2 not used by tree from Similarity
# $Id: Makefile,v 1.4 2011-02-28 23:54:51 simeon Exp $

OBJ=DocSet.o DocInfo.o Logger.o KgramInfo.o KeyMap.o MarkedDoc.o KeyTable.o KeyTable3Element.o DocPair.o kgrams.o options.o files.o pstats.o

LIBS=-lstdc++

CPP = gcc
CPPDEFS = -D__LINUX__
CPPFLAGS = -O -g -Wall -I.

.cpp.o:
	$(CPP) $(CPPDEFS) $(CPPFLAGS) -c $<

all: $(OBJ)

# Want checks in here
KeyTable.o: KeyTable.h KeyTable.cpp
	gcc $(CPPDEFS) $(CPPFLAGS) -DSTRICT_CHECKS -c KeyTable.cpp

DocSet.o: DocSet.cpp DocSet.h

DocInfo.o: DocInfo.cpp DocInfo.h

options.o: options.cpp options.h

test_KeyTable3Element: test_KeyTable3Element.cpp KeyTable3Element.cpp KeyTable3Element.h
	g++ $(CPPDEFS) $(CPPFLAGS) -o test_KeyTable3Element test_KeyTable3Element.cpp KeyTable3Element.cpp

clean:
	rm -f test_KeyTable3Element test_pstats
	rm -f $(OBJ)

distclean: clean
