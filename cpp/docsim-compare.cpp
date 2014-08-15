// docsim-compare.cpp
//
// Compare a new document with data in an existing set of extracted keys
// to report potential overlaps.
//
// Simeon Warner - 2005-08-10...

#include "definitions.h"
#include "options.h"
#include "Logger.h"
#include "DocSet.h"
#include "DocInfo.h"
#include "kgrams.h"
#include "files.h"
#include <fstream>

string myname="docsim-compare";

int main(int argc, char* argv[])
{
  VERBOSE=0;
  VERY_VERBOSE=0;

  // Read options using standard code for all of DocSim programs
  readOptions(argc, argv, (const char*)"d:o:f:p:m:St:T:b:n:", myname, "Compare a new document (-f) with data for a corpus in an existing map (-m, -t or -T). Writes out candidate overlapping documents (-p) that have at least keysForMatch (-n) overlapping keys");
 
  // Load new file and create KeyMap
  KeyMap newkeys;
  if (filename1=="") {
    cerr << myname << ": No filename specified, use -f <filename1>\n";
    exit(1);
  }
  string newdocFile=prependPath(dataDir,filename1);
  DocInfo newdoc(newdocFile,1);
  newdoc.addToKeymap(newkeys);
  string newdocKeyMapFile=prependPath(baseDir,"newdoc.keymap");
  ofstream ndout;
  ndout.open(newdocKeyMapFile.c_str(),ios_base::out);
  ndout << newkeys;
  ndout.close();
  cout << myname << ": read new doc, got " << newkeys.size() << " keys, KeyMap dumped to " << newdocKeyMapFile << endl;
  
  // Now, somehow get a KeyMap of the overlap. What we load depends on the command
  // line options...
  //
  // At the end of this if block we will have shared keys between new doc and
  // corpus in sharedkeys
  KeyMap sharedkeys;
  //
  if (keyMapFile!="") {
    // Load existsing KeyMap
    KeyMap allkeys;
    string fullKeyMapFile=prependPath(baseDir,keyMapFile);
    ifstream kin;
    if (VERY_VERBOSE) {
      cerr << myname << ": going to read keyMapFile " << fullKeyMapFile << endl;
    }
    kin.open(fullKeyMapFile.c_str(),ios_base::in);
    if (!kin.good()) {
      cerr << myname << ": Error - failed to open '" << fullKeyMapFile << "' to read KeyMap, aborting!" << endl;
      exit(2);
    }
    kin >> allkeys;
    kin.close();
    cout << myname << ": read corpus KeyMap with " << allkeys.size() << " keys." << endl;
    if (allkeys.size()==0) {
      cerr << myname << ": no keys read, aborting" << endl;
      exit(1);
    }
    filterKeymap(allkeys,newkeys,sharedkeys);
  } else if (keyTableFile!="" || keyTableBase!="") {
    KeyTable dummyKT(bitsInKeyTable,true);
    indexhashset indexes;
    dummyKT.keysToIndexes(newkeys,indexes);
    ofstream iout;
    string indexesFile=prependPath(baseDir,"newdoc.indexes");
    iout.open(indexesFile.c_str(),ios_base::out);
    dummyKT.writeIndexes(iout,indexes);
    iout.close();
    cout << myname << ": written newdoc indexes to " << indexesFile << endl;
    if (keyTableFile!="") {
      string fullKeyTableFile=prependPath(baseDir,keyTableFile);
      ifstream kin;
      kin.open(fullKeyTableFile.c_str(),ios_base::in);
      if (!kin.good()) {
        cerr << myname << ": Error - failed to open '" << fullKeyTableFile << "' to read KeyTable, aborting!" << endl;
        exit(2);
      }
      dummyKT.readTables123(kin,&indexes,&sharedkeys);
      kin.close();  
    } else { // if (keyTableBase!="") {
      string fullKeyTableBase=prependPath(baseDir,keyTableBase);
      dummyKT.readMultiFile(fullKeyTableBase,&indexes,&sharedkeys);
    }
  } else {
    cerr << myname << ": Error - didn't find keyMapFile (-m), keyTableFile (-t) or keyTableBase (-T)." << endl;
    exit(2);
  }
 
  cout << myname << ": " << sharedkeys.size() << " keys from new doc appear in corpus\n";

  string sharedkeysFile=prependPath(baseDir,"sharedkeys.txt");
  ofstream sout;
  sout.open(sharedkeysFile.c_str(),ios_base::out);
  sout << sharedkeys;
  sout.close();
  cout << myname << ": written shared keys KeyMap to " << sharedkeysFile << endl;

  DocPairVector dpv;
  sharedkeys.getCommonDocs(dpv, keysForMatch);
  string candidatesFilePath=prependPath(baseDir,candidatesFile);
  cout << myname << ": Writing " << dpv.size() << " overlapping (>=" << keysForMatch << " keys) docs to " << candidatesFilePath << endl;
  ofstream cdout;
  cdout.open(candidatesFilePath.c_str(),ios_base::out);
  cdout << dpv;
  cdout.close();
  
  return 0;
}
