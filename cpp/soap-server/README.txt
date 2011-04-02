
1) get gsoap from http://gsoap2.sourceforge.net/

2) unpack, e.g.

tar -zxf gsoap-2.7.12.tar.gz
mv gsoap-2.7 gsoap-2.7.12
cd gsoap-2.7.12

3) configure, make, install

sudo mkdir /opt/gsoap-2.7.12
./configure --prefix=/opt/gsoap-2.7.12
make
sudo make install

4) The step above installs the compiled code, libraries and header files 
in /opt/gsoap-2.7.12. It does not install parts of the code that 
need to be included to compile programs with gsoap. These are
installed locally under the local directory gsoap so that this code
can be can also be compiled with standard system libraries skipping
step 3. Copy from a new distribution with:

cp gsoap-2.7.12/gsoap/stdsoap2.cpp gsoap
cp gsoap-2.7.12/gsoap/stdsoap2.h gsoap
cp gsoap-2.7.12/gsoap/stdsoap2.c gsoap

5) edit Makefile to point to installed copy of gsoap bin/libs and the stdsoap2
headers:

...
GSOAP_PREFIX=/opt/gsoap-2.7.12
GSOAP_SRC=gsoap
...

6) Build soap server:

make clean
make

7) Check binary runs:

./overlapd -h

8) Run test code in main docsim dir, see ../README.txt


$Id: README.txt,v 1.1 2008-11-18 17:58:10 simeon Exp $


