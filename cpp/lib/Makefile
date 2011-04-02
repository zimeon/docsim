# Sentence used only by SimSearch2
# SimSearch2 not used by tree from Similarity
# $Id: Makefile,v 1.6 2011-04-02 16:28:19 simeon Exp $

OBJ=DocSet.o DocInfo.o Logger.o KgramInfo.o KeyMap.o MarkedDoc.o KeyTable.o KeyTable3Element.o DocPair.o kgrams.o options.o files.o pstats.o

LIBS=-lstdc++

# gcc 4.3 complains about use of deprecated hash_set and hash_map, suggesting instead
# use of unordered_set and unordered_map. However, to use these one must enable
# "compiler and library support for the upcoming ISO C++ standard, C++0x. This support
# is currently experimental". Thus, for now, add -Wno-depreacted to avoid warnings.
CPP = gcc
CPPDEFS = -D__LINUX__
CPPFLAGS = -O -g -Wno-deprecated -I.

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
	g++ $(CPPDEFS) -DSTRICT_CHECKS $(CPPFLAGS) -o test_KeyTable3Element test_KeyTable3Element.cpp KeyTable3Element.cpp

clean:
	rm -f test_KeyTable3Element test_pstats
	rm -f $(OBJ)

distclean: clean
