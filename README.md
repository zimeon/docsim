# docsim -- Document similarity programs

> **This code is archived and there are no plans for further work.**
> This code was developed to perform text overlap detection for arXiv.org
> around 2011. Much of the work was to minimize the memory footprint of
> truncated hashes for the several hundred-thousand document arXiv
> corpus. Having this in-memory was necessary to implement a daemon
> that could quickly compare any new document with the rest of the corpus,
> and machines at the time had only a few GB of RAM. I suspect that any
> new effort with such analysis would pick different trade-offs given
> increased memory available. _[Simeon/2023-09-14]_

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

This will build the command line docsim tools but will not build
the SOAP-based `overlapd` daemon.

## gSOAP library

The gSOAP library is needed in order to compile to `overlapd` 
daemon that provides SOAP-based communication with a memory resident 
overlap corpus.

### RedHat (RHEL6)

As of 2014-07 the standard RHEL6 packages for `gsoap` and `gsoap-devel`
are version 2.7.16. These may be installed with:

```
sudo yum install gsoap gsoap-devel
```

This installs in the system paths with prefix `/usr` (ie. shared 
libraries in `/usr/lib`, headers in `/usr/include`, binaries in
`/usr/bin`). The `Makefile` in `cpp/soap-server` is set up for these
locations. From the root directory, make `overlapd` with:

```
make soap
```

### Debian and derivatives

*Warning - notes from 2007 and may be out of date*

On Debian and derivative systems (e.g. Ubuntu) this should be
available for etch and later distributions via apt. Install with, e.g.

```
sudo apt-get install gsoap
```

In March 2007 the current version on etch was 2.7.6 which was fine and
apt installed it under `/usr`. The `Makefile` in `cpp/soap-server` 
is set up for this location.

### Other systems

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

For a simple test run with data supplied. First create analyze a set of
testdata to build a keys file:

```
cpp/docsim-analyze -d testdata/arxiv-publicdomain -f testdata/arxiv-publicdomain/files.txt -b 20
```

And then compare one of the documents against the corpus:

```
cpp/docsim-compare -f testdata/arxiv-publicdomain/0012/math0012129.txt.gz -b 20 -T /tmp/allkeys
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
