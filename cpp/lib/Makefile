# Docsim libraries
#
OBJ=DocSet.o DocInfo.o Logger.o KgramInfo.o KeyMap.o MarkedDoc.o KeyTable.o KeyTable3Element.o DocPair.o kgrams.o options.o files.o pstats.o

LIBS=-lstdc++

CPP = gcc
CPPDEFS =
CPPFLAGS = -O -Wall -I.

.cpp.o:
	$(CPP) $(CPPDEFS) $(CPPFLAGS) -c $<

all: $(OBJ)


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
