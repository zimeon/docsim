# Makefile for Docsim libraries
#

OBJ=DocSet.o DocInfo.o DocPair.o Logger.o KgramInfo.o KeyMap.o MarkedDoc.o KeyTable.o KeyTable3Element.o DocPair.o kgrams.o options.o files.o pstats.o anystream.o

STDLIBS=-lstdc++

CPP = gcc
# For gcc < 4.3 must add -D__NO_TR1__ to CPPDEFS, gcc >= 4.3 omit and use std::tr1::unordered_set|map instead of hash_set|map
# within Docsim this would usuall come via $(GLOBAL_CPPDEFS)
CPPDEFS = -DSTRICT_CHECKS $(GLOBAL_CPPDEFS)
#CPPDEFS = -D__NO_TR1__ -DSTRICT_CHECKS
CPPFLAGS = -g -O -Wall -I .

%.o: %.cpp %.h
	$(CPP) $(CPPDEFS) $(CPPFLAGS) -c $<

%.o: %.cpp
	$(CPP) $(CPPDEFS) $(CPPFLAGS) -c $<

all: $(OBJ)

anystream.o: anystream.h anystream.cpp ../include/gzstream.h
	$(CPP) $(CPPDEFS) $(CPPFLAGS) -I ../include -c anystream.cpp

.PHONY: test
test: test_KeyTable3Element test_pstats test_kgrams test_KgramInfo test_KeyTable test_KeyMap

test_KeyTable3Element: test_KeyTable3Element.cpp KeyTable3Element.cpp KeyTable3Element.h
	g++ $(CPPDEFS) $(CPPFLAGS) -o test_KeyTable3Element test_KeyTable3Element.cpp KeyTable3Element.cpp

test_pstats: test_pstats.cpp pstats.cpp pstats.h
	g++ $(CPPDEFS) $(CPPFLAGS) -o test_pstats test_pstats.cpp pstats.cpp

test_kgrams: test_kgrams.o options.o kgrams.o 
	gcc $(CPPFLAGS) $(STDLIBS) -o test_kgrams test_kgrams.o kgrams.o options.o

test_KgramInfo: test_KgramInfo.o KgramInfo.o kgrams.o options.o
	gcc $(CPPFLAGS) $(STDLIBS) -o test_KgramInfo test_KgramInfo.o KgramInfo.o kgrams.o options.o

test_KeyTable: test_KeyTable.cpp KeyTable.o KeyTable3Element.o KeyTable.h kgrams.o KeyMap.o KgramInfo.o options.o pstats.o DocPair.o
	gcc $(CPPDEFS) $(CPPFLAGS) -DSTRICT_CHECKS -c test_KeyTable.cpp
	gcc $(CPPFLAGS) $(STDLIBS) -o test_KeyTable test_KeyTable.o options.o kgrams.o KeyTable.o KeyTable3Element.o KeyMap.o KgramInfo.o DocPair.o pstats.o

test_KeyMap: test_KeyMap.o KeyMap.o KgramInfo.o kgrams.o DocPair.o options.o test_more.o
	gcc $(CPPFLAGS) $(STDLIBS) -o test_KeyMap test_KeyMap.o KeyMap.o KgramInfo.o kgrams.o DocPair.o options.o test_more.o

clean:
	rm -f test_KeyTable3Element.o test_pstats.o test_kgrams.o test_KgramInfo.o test_KeyTable.o test_KeyMap.o
	rm -f test_KeyTable3Element test_pstats test_kgrams test_KgramInfo test_KeyTable test_KeyMap
	rm -f $(OBJ)

distclean: clean
