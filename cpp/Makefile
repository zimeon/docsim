# Makefile for C++ code in DocSim suite
# Simeon Warner - 2005-2011

DOCSIMLIBS=lib/DocSet.o lib/DocInfo.o lib/Logger.o lib/KgramInfo.o lib/KeyMap.o lib/MarkedDoc.o lib/KeyTable.o lib/KeyTable3Element.o lib/DocPair.o lib/kgrams.o lib/options.o lib/files.o lib/pstats.o

LIBSLACK=/usr/local/lib/libslack.a

STDLIBS=-lstdc++

CPP = gcc
CPPDEFS =
CPPFLAGS = -g -O -Wall -I . -I lib

.cpp.o:
	$(CPP) $(CPPDEFS) $(CPPFLAGS) -c $<

all: docsimlibs docsim-analyze docsim-compare docsim-concat docsim-overlap findkgram kgramkey overlapd

docsimlibs:
	cd lib && make

findkgram: $(DOCSIMLIBS) findkgram.o
	gcc $(CPPFLAGS) -o findkgram findkgram.o $(DOCSIMLIBS) $(STDLIBS)

kgramkey: $(DOCSIMLIBS) kgramkey.o
	gcc $(CPPFLAGS) -o kgramkey kgramkey.o $(DOCSIMLIBS) $(STDLIBS)

docsim-analyze: $(DOCSIMLIBS) docsim-analyze.o
	gcc $(CPPFLAGS) -o docsim-analyze docsim-analyze.o $(DOCSIMLIBS) $(STDLIBS)

docsim-compare: $(DOCSIMLIBS) docsim-compare.o
	gcc $(CPPFLAGS) -o docsim-compare docsim-compare.o $(DOCSIMLIBS) $(STDLIBS)

docsim-concat: $(DOCSIMLIBS) docsim-concat.o
	gcc $(CPPFLAGS) -o docsim-concat docsim-concat.o $(DOCSIMLIBS) $(STDLIBS)

docsim-overlap: $(DOCSIMLIBS) docsim-overlap.o
	gcc $(CPPFLAGS) -o docsim-overlap docsim-overlap.o $(DOCSIMLIBS) $(STDLIBS)

overlapd:
	cd soap-server && make

# Create a version of docsim that just writes out stats about the
# documents it processes, instead of actually building up a 
# keymap of the keys.
#
docsim_stats: docsimlibs
	gcc $(CPPDEFS) $(CPPFLAGS) -DDOCUMENT_STATS -c DocInfo.cpp
	gcc $(CPPFLAGS) -DDOCUMENT_STATS -o docsim_stats $(DOCSIMLIBS) $(STDLIBS)
	rm -f DocInfo.o

test_kgrams: test_kgrams.o docsimlibs
	gcc $(CPPFLAGS) -o test_kgrams test_kgrams.o lib/kgrams.o lib/options.o $(STDLIBS)

test_KgramInfo: test_KgramInfo.o KgramInfo.o kgrams.o definitions.o
	gcc $(CPPFLAGS) -o test_KgramInfo KgramInfo.o kgrams.o definitions.o test_KgramInfo.o $(STDLIBS)

DocSet.o: DocSet.cpp DocSet.h definitions.h

DocInfo.o: DocInfo.cpp DocInfo.h definitions.h

KgramInfo.o: KgramInfo.cpp KgramInfo.h definitions.h

MarkedDoc.o: MarkedDoc.cpp MarkedDoc.h definitions.h

Logger.o: Logger.cpp Logger.h definitions.h

files.o: files.cpp files.h definitions.h

test_KeyTable: test_KeyTable.cpp lib/KeyTable.cpp lib/KeyTable3Element.o lib/KeyTable.h lib/kgrams.o lib/KeyMap.o lib/KgramInfo.o lib/options.o lib/pstats.o
	gcc $(CPPDEFS) $(CPPFLAGS) -DSTRICT_CHECKS -c lib/KeyTable.cpp
	gcc $(CPPDEFS) $(CPPFLAGS) -DSTRICT_CHECKS -c test_KeyTable.cpp
	gcc $(CPPFLAGS) -o test_KeyTable test_KeyTable.o lib/options.o lib/kgrams.o lib/KeyTable.o lib/KeyTable3Element.o lib/KeyMap.o lib/KgramInfo.o lib/DocPair.o lib/pstats.o $(STDLIBS)
	#rm KeyTable.o

####

test:
	make test1_analyse
	@echo -e "\n\n==== STARTING overlapd ====\n\n";
	make test1_run_overlapd &
	sleep 2
	@echo -e "\n\n==== RUNNING test1_query1 AGAINST overlapd ====\n\n";
	make test1_query1 || killall overlapd
	@echo -e "\n\n==== RUNNING test1_query2 AGAINST overlapd ====\n\n";
	make test1_query2 || killall overlapd
	@echo -e "\n\n==== KILLING overlapd ====\n\n";
	killall overlapd
	sleep 2
	@echo -e "\n\n";


#### A set of commands to setup test1

test1_analyse: docsim-analyze
	./docsim-analyze -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt -b 20 -o tmp

test1_run_overlapd: overlapd
	./soap-server/overlapd -b 20 -T tmp/allkeys

test1_query1: test-overlap_server.pl
	./test-overlap_server.pl -q testdata/arxiv-cs-500/cs/9903/9903001.psv -f testdata/arxiv-cs-500/files.txt -d testdata/arxiv-cs-500/

test1_query2: test-overlap_server.pl
	./test-overlap_server.pl -q testdata/arxiv-cs-500/cs/9903/9903002.psv -f testdata/arxiv-cs-500/files.txt -d testdata/arxiv-cs-500/


test2: test2_analyze

test2_analyze:
	./docsim-analyze -d testdata -f testdata/arxiv-test1/psvfiles10.txt -v


test3: test3_findkgram

test3_findkgram:
	./findkgram -K similar/shared_keys -f /data/psv/hep-ph/0311/0311168.psv
	./findkgram -K similar/shared_keys -f /data/psv/hep-ph/0506/0506241.psv


.PHONY: clean
clean:
	rm -f docsim-analyze docsim-analyze.o
	rm -f docsim-compare docsim-compare.o
	rm -f docsim-overlap docsim-overlap.o
	rm -f docsim-concat docsim-concat.o
	rm -f findkgram findkgram.o
	rm -f kgramkey kgramkey.o
	rm -f test_kgrams test_kgrams.o
	rm -f test_KeyTable test_KeyTable.o
	cd lib && make clean
	cd soap-server && make clean

.PHONY: tar
tar:
	cd ..; tar --exclude="docsim/testdata" --exclude="docsim/gsoap*" --exclude=CVS --exclude=".#*" --exclude="*~" --exclude="*.o" -zcf /tmp/docsim_no_gsoap_or_testdata.tar.gz docsim; cd docsim 
	ls -l /tmp/docsim_no_gsoap_or_testdata.tar.gz
	@echo "Run make tar-full to include gsoap and testdata"

.PHONY: tar-full
tar-full:
	cd ..; tar --exclude=CVS --exclude=".#*" --exclude="*~" --exclude="*.o" -zcf /tmp/docsim.tar.gz docsim; cd docsim 
	ls -l /tmp/docsim.tar.gz
	@echo "Run make tar to exclude gsoap and testdata"

