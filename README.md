# docsim -- Document similarity programs

## Sentence and file based analysis

This code may be run either ignoring linebreaks in input text files and 
treating the whole input file as one sentence. or respecting linebreaks 
in the input files as indicating sentences (as for [SOROKINA+07]). The 
default is to treat input files as a whole, ignoring sentence 
boundaries.

To select respecting sentences use the -S option for docsim-analyze, 
docsim-compare, and overlapd. Note that you will probably get odd 
results if there is mismatch in respect for sentences or not between 
analysis and comparison.

Treating files as a whole means that lib/files.cpp needs a buffer that
can read in a complete input file. The buffer size is controlled in
lib/definitions.h:

```
#define FILE_BUFFER_SIZE 5000000
```

As of 2011-02-16 the maximum psv file size for arXiv is ~4.1MB so a 
buffer size larger than this is required to treat all files whole.

## Compilation

The C++ code in directory ccp and under was developed with gcc 4.1.2.
It compiles with gcc 4.4 but throws some deprecated header warnings. 
(2011-03-03)

To compile:

```
> cd cpp
> make
```

## gSOAP library

On Debian and derivative systems (e.g. Ubuntu) this should be
available for etch and later distributions via apt. Install with, e.g.

```
sudo apt-get install gsoap
```

In March 2007 the current version on etch was 2.7.6 which was fine and
apt installed it in /usr/lib.

For other systems, try sourceforge:
<http://sourceforge.net/projects/gsoap2/>


## Other libraries

Gzstream - this library was downloaded from 
<http://www.cs.unc.edu/Research/compgeom/gzstream/>
on 2011-08-25 and is included within the cpp/include directory. It is 
used to allow the code to read plain or gzip format data files (to allow 
disk space saving). The make files for Docsim will compile this code.
This is LGPL licensed.


## Testing

For a simple test run with data supplied:

```
./docsim-analyze -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt -b 20
./docsim-compare -f testdata/arxiv-cs-500/cs/9912/9912018.psv -b 20 -T /tmp/allkeys
```


## Credits

These programs extend work reported in "Plagiarism Detection in
arXiv", Daria Sorokina, Johannes Gehrke, Simeon Warner, Paul Ginsparg
[ICDM'06, <http://arxiv.org/abs/cs/0702012>]. Most of these programs
have been written by Simeon Warner between 2005 and 2009. They include
portions based on code written by and copyright Daria Sorokina, 2005
as noted in the source files.


## References

[SOROKINA+07] Daria Sorokina, Johannes Gehrke, Simeon Warner, Paul
Ginsparg. "Plagiarism Detection in arXiv" doi:10.1109/ICDM.2006.126
<http://arxiv.org/abs/cs/0702012>