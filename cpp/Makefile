# Makefile for C++ code in DocSim suite
# Simeon Warner - 2005-2014

DOCSIMLIBS=lib/DocSet.o lib/DocInfo.o lib/Logger.o lib/KgramInfo.o lib/KeyMap.o lib/MarkedDoc.o lib/KeyTable.o lib/KeyTable3Element.o lib/DocPair.o lib/kgrams.o lib/options.o lib/files.o lib/pstats.o lib/anystream.o include/gzstream.o

LIBSLACK=/usr/local/lib/libslack.a

STDLIBS=-lstdc++

CPP = gcc
# For gcc < 4.3 must add -D__NO_TR1__ to CPPDEFS, gcc >= 4.3 omit and use std::tr1::unordered_set|map instead of hash_set|map
# within Docsim	this would usuall come via $(GLOBAL_CPPDEFS)
CPPDEFS =
#CPPDEFS = -D__NO_TR1__
GLOBAL_CPPDEFS = $(CPPDEFS)
export GLOBAL_CPPDEFS
CPPFLAGS = -g -O -Wall -I . -I lib -I include -lz

# Configuration for tests
TESTDATA = ../testdata/arxiv-publicdomain
TESTTMP  = /tmp

.cpp.o:
	$(CPP) $(CPPFLAGS) $(CPPDEFS) -c $<

.PHONY: main
main: docsimlibs docsim-analyze docsim-compare docsim-concat docsim-overlap findkgram kgramkey

.PHONY: soap
soap: docsimlibs overlapd

.PHONY: all
all: main soap

docsimlibs:
	cd lib && make
	cd include && make

findkgram: docsimlibs findkgram.o
	gcc $(CPPFLAGS) -o findkgram findkgram.o $(DOCSIMLIBS) $(STDLIBS)

kgramkey: docsimlibs kgramkey.o
	gcc $(CPPFLAGS) -o kgramkey kgramkey.o $(DOCSIMLIBS) $(STDLIBS)

docsim-analyze: docsimlibs docsim-analyze.o
	gcc $(CPPFLAGS) -o docsim-analyze docsim-analyze.o $(DOCSIMLIBS) $(STDLIBS)

docsim-compare: docsimlibs docsim-compare.o
	gcc $(CPPFLAGS) -o docsim-compare docsim-compare.o $(DOCSIMLIBS) $(STDLIBS)

docsim-concat: docsimlibs docsim-concat.o
	gcc $(CPPFLAGS) -o docsim-concat docsim-concat.o $(DOCSIMLIBS) $(STDLIBS)

docsim-overlap: docsimlibs docsim-overlap.o
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

DocSet.o: DocSet.cpp DocSet.h definitions.h

DocInfo.o: DocInfo.cpp DocInfo.h definitions.h

KgramInfo.o: KgramInfo.cpp KgramInfo.h definitions.h

MarkedDoc.o: MarkedDoc.cpp MarkedDoc.h definitions.h

Logger.o: Logger.cpp Logger.h definitions.h

files.o: files.cpp files.h definitions.h

####

.PHONY: test-all
test-all: test test-overlapd

.PHONY: test
test:
	@echo "Running functional tests in Perl (get details with prove -v)..."
	prove

.PHONE: test-overlapd
test-overlapd:
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
	@echo "Building test KeyTable from files in $(TESTDATA)/files.txt"	
	./docsim-analyze -d $(TESTDATA) -f $(TESTDATA)/files.txt -b 20 -o $(TESTTMP)
	mv $(TESTTMP)/allkeys_1.keytable $(TESTTMP)/test1_allkeys_1.keytable

test1_compare_doc1: docsim-compare 
	@echo "Use docsim-compare to find an exact match document in the corpus from KeyTable"
	./docsim-compare -T $(TESTTMP)/test1_allkeys -b 20 -f $(TESTDATA)/1012/1012.5086.txt.gz

test1_run_overlapd: overlapd
	./soap-server/overlapd -b 20 -T $(TESTTMP)/test1_allkeys

test1_query1: test-overlap_server.pl
	./test-overlap_server.pl -q $(TESTDATA)/cs/9903/9903001.psv -f $(TESTDATA)/files.txt -d $(TESTDATA)/

test1_query2: test-overlap_server.pl
	./test-overlap_server.pl -q $(TESTDATA)/cs/9903/9903002.psv -f $(TESTDATA)/files.txt -d $(TESTDATA)/


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
	cd include && make clean
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

