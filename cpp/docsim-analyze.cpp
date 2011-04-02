// docsim-analyze.cpp
// Based in part on Similarity.cpp from Daria Sorokina
// Simeon Warner - 2005-07...
// 
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


const string myname="docsim-analyze";

int main(int argc, char* argv[])
{
  VERBOSE=0;
  VERY_VERBOSE=0;
  //
  // Read options using standard code for all of docsim programs
  readOptions(argc, argv, "d:o:f:b:cr:ST:x:X:", myname, "<filename1> is file containing a list of filenames of normalized txt files to read (relative to the data directory (-d)). Will write a keymap by default but a KeyTable if the -b option is specified to give the number of bits. If -T keyTableBase is given then this KeyTable will be read in before adding more documents. ");

  // Read list of psv files to work with
  cout << myname << ": about to run " << myname << "...\n";

  string logFile=prependPath(baseDir,"log.txt");
  string warnFile=prependPath(baseDir,"warn.txt");
  Logger log;
  log.init(logFile.c_str(),1,warnFile.c_str(),0);

  DocSet docs;
  docs.readFileList(filename1,dataDir);
  cout << myname << ": read list of " << docs.size() << " psv files\n";

  // Now look to see if we are to process just a range of the ids
  // the document set
  int cStart=-1;
  int cEnd=-1;
  bool justARange=false;
  string rangeId="";
  if (rangeStart>0 || rangeEnd>0) {
    cStart=0;
    cEnd=(docs.size()-1);
    if (rangeStart>0) cStart=rangeStart;
    if (rangeEnd>0) cEnd=rangeEnd;
    justARange=true;
    // Build a range id for the output files if this doesn't go to the end
    if (rangeEnd>0) {
      ostringstream rstr;
      rstr << "_" << rangeStart << "_"<< rangeEnd;
      rangeId = rstr.str();
    }
    cout << myname << ": Request to process range from file " << cStart << " to " << cEnd << endl;
  }

  if (!justARange || cStart==0) {
    string docidsFile=prependPath(baseDir,"docids.txt");
    ofstream diout;
    diout.open(docidsFile.c_str(),ios_base::out);
    diout << docs;
    diout.close();
  }

  if (bitsInKeyTable>0) {

    cout << myname << ": Will create KeyTable using " << bitsInKeyTable << " bit keys" << endl;
    KeyTable keytable(bitsInKeyTable,false,selectBits,selectMatch);

    // Was an existing keytable specified to start from?
    if (keyTableBase!="") {
      keytable.readMultiFile(keyTableBase);
    }

    // Add keys from the selected set of documents
    docs.addToKeyTable(keytable, -1, cStart, cEnd);

    // Write full set of KeyTable files
    ofstream ktout;
    string keytableBaseName=prependPath(baseDir,"allkeys"+rangeId);
    cout << myname << ": Writing KeyTable to files starting " << keytableBaseName << endl;
    int nf1=keytable.writeMultiFile(keytableBaseName,true);
    cout << myname << ": Finished writing " << nf1 << " KeyTable to files starting " << keytableBaseName << endl;

    // Drop table1 (keys appearing only once) and write again
    keytable.dropTable1();
    cout << myname << ": Dropped table1, KeyTable stats:" << endl;
    keytable.writeStats(cout);
  
    string keytableBaseName2=prependPath(baseDir,"sharedkeys"+rangeId);
    cout << myname << ": Writing KeyTable2 to " << keytableBaseName2 << endl;
    int nf2=keytable.writeMultiFile(keytableBaseName2,false);
    cout << myname << ": Finished writing KeyTable2 in " << nf2 << " files to files starting " << keytableBaseName2 << endl;
    
    if (compare) {
      intv oids;
      keytable.getOverlapIds(oids, 20);
      cout << myname << ": Got " << oids.size() << " document ids with overlap >=20" << endl;
      DocPairVector dpv;
      keytable.getOverlapDocs(dpv, oids, 20);
      string candidateFile=prependPath(baseDir,"candidate"+rangeId+".txt");
      cout << myname << ": Writing overlapping (>=20 keys) docs to " << candidateFile << endl;
      ofstream cdout;
      cdout.open(candidateFile.c_str(),ios_base::out);
      cdout << dpv;
      cdout.close();
    }

  } else { // use keymap
    keymap allkeys;
    docs.getKeymap(allkeys, MAX_DUPES_TO_COUNT, true, cStart, cEnd);
    cout << myname << ": built keymap, " << allkeys.size() << " keys\n";

    keymap commonkeys;
    docs.stripCommon(allkeys, commonkeys, NUM_DUPES_TO_BE_COMMON);
    cout << myname << ": stripped common, left " << allkeys.size() << " keys\n";
    cout << myname << ": got " << commonkeys.size() << " common keys\n";

    string allkeysFile=prependPath(baseDir,"allkeys"+rangeId+".txt");
    ofstream akout;
    akout.open(allkeysFile.c_str(),ios_base::out);
    akout << allkeys;
    akout.close();

    string commonkeysFile=prependPath(baseDir,"commonkeys"+rangeId+".txt");
    ofstream ckout;
    ckout.open(commonkeysFile.c_str(),ios_base::out);
    ckout << commonkeys;
    ckout.close();
  }

  return 0;
}
