# Makefile for DocSim suite
# Simeon Warner - 2005-2014

.PHONY: main
main: cpp-main

.PHONY: all
all: cpp-main cpp-soap

.PHONY: soap
soap: cpp-soap
 
.PHONY: cpp-main
cpp-main:
	@echo "===> Building main C++ code"
	cd cpp && make main
	@echo "(use make soap to also build the SOAP server)"

.PHONY: cpp-soap
cpp-soap:
	@echo "===> Building SOAP server C++ code"
	cd cpp && make soap

.PHONY: clean
clean:
	cd cpp && make clean

#===========================================================
# 
VALGRIND = /usr/bin/valgrind

soap_server_leak_test: testdata $(VALGRIND) cpp
	@echo "===> Doing memory leak test (may take some time)"
	@echo "=> Running analysis to get test data"
	cpp/docsim-analyze -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt -b 20
	@echo "=> Starting server"
	rm /tmp/overlapd.log
	$(VALGRIND) --leak-check=yes cpp/soap-server/overlapd -b 20 -t /tmp/allkeys_1.keytable &
	perl/docsim-overlap-server-wait.pl -t 180
	@echo "=> Server happy, running tests"
	perl/docsim-overlap-server-query.pl -S 1000 -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt
	@echo "=> The following grep of log output should show steady data size" 
	-grep data= /tmp/overlapd.log
	@echo "=> The valgrind output should not report any memory leaks"
	-kill -INT `pidof -s $(VALGRIND).bin`
	@echo "=> Done, check output above for memory issues"

testdata:
	@echo "Error - testdata directory does not exist"
	exit(1)