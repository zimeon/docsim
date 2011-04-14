// docsim-conact.cpp
//
// Read and concatenate (combine) data from a set of KeyTable
// files.
//
// Simeon Warner - 2009-11-21...

#include "definitions.h"
#include "options.h"
#include "Logger.h"
#include "DocSet.h"
#include "DocPair.h"
#include "kgrams.h"
#include "files.h"
#include <unistd.h> // for GNU getopt
#include <fstream>
#include <sstream>


const string myname="docsim-concat";

int main(int argc, char* argv[])
{
  VERBOSE=0;
  VERY_VERBOSE=0;
  //
  // Read options using standard code for all of docsim programs
  int next_arg=readOptions(argc, argv, "d:o:f:b:cr:T:x:X:", myname, "<filename1> is file containing a list of filenames of normalized txt files to read (relative to the data directory (-d)). The number of bits in the KeyTable must be specified with the -b option. If -T keyTableBase is given then this KeyTable will be read in before adding more documents. ");

  // Read list of psv files to work with
  cout << myname << ": about to run " << myname << "...\n";

  if (bitsInKeyTable<=0) {
    cerr << myname << ": Must specify bit in KeyTable, aborting!" << endl;
  }

  KeyTable keytable(bitsInKeyTable);

  keytable.setPruneAbove(10);
  for (;next_arg<argc; next_arg++) {
    string ktFile = prependPath(baseDir,argv[next_arg]);
    cout << "Reading KeyTable '" << ktFile << "'" << endl;
    keytable.readMultiFile(ktFile);
    keytable.writeStats(cout);
  }

  // Was an existing keytable specified to start from?
  if (keyTableBase=="") {
    cerr << "No place to write specified, stopping" << endl;
  }

  // Write full set of KeyTable files
  ofstream ktout;
  string keytableBaseName=prependPath(baseDir,"allkeys_concat");
  cout << myname << ": Writing KeyTable to files starting " << keytableBaseName << endl;
  int nf1=keytable.writeMultiFile(keytableBaseName,true);
  cout << myname << ": Finished writing " << nf1 << " KeyTable to files starting " << keytableBaseName << endl;

  // Drop table1 (keys appearing only once) and write again
  keytable.dropTable1();
  cout << myname << ": Dropped table1, KeyTable stats:" << endl;
  keytable.writeStats(cout);
  
  string keytableBaseName2=prependPath(baseDir,"sharedkeys_concat");
  cout << myname << ": Writing KeyTable2 to " << keytableBaseName2 << endl;
  int nf2=keytable.writeMultiFile(keytableBaseName2,false);
  cout << myname << ": Finished writing KeyTable2 in " << nf2 << " files to files starting " << keytableBaseName2 << endl;

  return 0;
}
