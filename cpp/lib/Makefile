# Makefile for Docsim libraries
#

OBJ=DocSet.o DocInfo.o Logger.o KgramInfo.o KeyMap.o MarkedDoc.o KeyTable.o KeyTable3Element.o DocPair.o kgrams.o options.o files.o pstats.o anystream.o

LIBS=-lstdc++

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

test_KeyTable3Element: test_KeyTable3Element.cpp KeyTable3Element.cpp KeyTable3Element.h
	g++ $(CPPDEFS) $(CPPFLAGS) -o test_KeyTable3Element test_KeyTable3Element.cpp KeyTable3Element.cpp

test_pstats: test_pstats.cpp pstats.cpp pstats.h
	g++ $(CPPDEFS) $(CPPFLAGS) -o test_pstats test_pstats.cpp pstats.cpp

clean:
	rm -f test_KeyTable3Element test_pstats
	rm -f $(OBJ)

distclean: clean
