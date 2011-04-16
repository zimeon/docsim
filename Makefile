# Makefile for Docsim suite
# Simeon Warner

.PHONY: all
all:  cpp

.PHONY: cpp
cpp:
	@echo "===> Building C++ code"
	cd cpp && make

.PHONY: clean
clean:
	cd cpp && make clean

#===========================================================
# 
VALGRIND = /usr/bin/valgrind

soap_server_leak_test: testdata $(VALGRIND) cpp
	@echo "===> Doing memory leak test (may take some time)"
	cpp/docsim-analyze -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt -b 20
	rm /tmp/overlapd.log
	$(VALGRIND) --leak-check=yes cpp/soap-server/overlapd -b 20 -t /tmp/allkeys_1.keytable &
	# allow time for server to start, should add option to docsim-overlap-server-query.pl that
	# wait for up to some time for the server to start
	sleep 180
	perl/docsim-overlap-server-query.pl -S 1000 -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt
	@echo "=> The following grep of log output should show steady data size" 
	-grep data= /tmp/overlapd.log
	@echo "=> The valgrind output should nore report any memory leaks"
	-kill -INT `pidof -s $(VALGRIND).bin`
	@echo "=> Done, check output above for memory issues"

testdata:
	@echo "Error - testdata directory does not exist"
	exit(1)