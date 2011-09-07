// A set of documents held as an array with associated methods
// Simeon Warner - 2005-07...
//
// $Id: DocSet.cpp,v 1.2 2011-02-09 21:07:14 simeon Exp $

#include "definitions.h"
#include "options.h"
#include "files.h"
#include "DocSet.h"
#include <fstream>

DocSet::DocSet(void)
{
}

DocSet::~DocSet(void)
{
}

// Returns reference to DocInfo obect for document with given docid
//
// DocInfo objects are actually stored in an array from 0...(n-1)
// but docids run 1...n.
//
DocInfo& DocSet::operator[](docid id)
{
  if (id<1) {
    cerr << "DocSet::operator[" << id <<" ]: Error - attempt to access id<1" << endl;
    exit(2);
  }
  if (id>docv.size()) {
    cerr << "DocSet::operator[" << id <<" ]: Error - attempt to access id>size()" << endl;
    exit(2);
  }
  return(docv[id-1]);
}


// Reads a list of files to process, each line of the list should have the
// following format:
//
// id psvFile absFile
//
void DocSet::readFileList(const string filename, const string dataDir)
{
  ifstream fin;
  fin.open(filename.c_str(),ios_base::in);
  if (!fin.is_open()) {
    cerr << "Error - failed to open '" << filename << "' for input\n";
    exit(1);
  }
  int n=0;
  
  while (!fin.eof()) {
    ++n;
    string psvFile;
    fin >> psvFile;
    if (psvFile.size()>0) {
      if (VERY_VERBOSE) {
        cout << "DocSet::readFileList[" << n << "]: " << psvFile << " (vv)" << endl;
      } else if (VERBOSE && n%1000==0) {
        cout << "DocSet::readFileList[" << n << "]: " << psvFile << endl;
      }
      DocInfo doc;
      doc.filename=prependPath(dataDir,psvFile);
      doc.id=n;
      docv.push_back(doc);      
    }
  }
  fin.close();
}


docid DocSet::addFile(const string psvFile, const string dataDir)
{
  if (VERY_VERBOSE) cout << "DocSet::addFile: " << psvFile << " (vv)" << endl;
  DocInfo doc;
  doc.filename=prependPath(dataDir,psvFile);
  doc.id=docv.size()+1;
  docv.push_back(doc);      
  return(doc.id);
}


//-------------------------------------------------------------------------
// keymap methods
//-------------------------------------------------------------------------


void DocSet::getKeymap(keymap& allkeys, int maxKeysToCount, bool winnow, int startFile, int endFile) {
  int i=0;	//number of documents in list
  int inc=0;	//number of documents included
  for (DocInfoVector::iterator docit=docv.begin();docit!=docv.end();docit++) {
    i++;
    if (((startFile<0) || (i>=startFile)) && ((endFile<0) || (i<=endFile))) {
      inc++;
      docit->addToKeymap(allkeys, maxKeysToCount, winnow);
      if (VERY_VERBOSE) cout << "DocSet::getKeymap[" << i << "]: " << docit->filename << " (vv)" <<endl;
      if (i%100==0) cout << "DocSet::getKeymap[" << i << "]: " << docit->filename << endl;
    }
  }	
  if (VERBOSE) cout << "DocSet::getKeymap: read " << inc << " files, got " << allkeys.size() << " keys " << endl;
}


void DocSet::stripCommon(keymap& allkeys, keymap& common, int numDupesToBeCommon)
{
  //cout << "DocSet::stripCommon: staring work on " << allkeys.size() << " keys" << endl;
  for (keymap::iterator kit = allkeys.begin(); kit != allkeys.end(); kit++) {
    if (kit->second->size() >= numDupesToBeCommon) {
      common.insert(keymap::value_type(kit->first,kit->second));
    }
  } //for over keys
  //
  // Now go through and delete the things we put in common
  // (if we stupidly try to do this in the loop above it gets very messy!)
  //
  for (keymap::iterator kit = common.begin(); kit != common.end(); kit++) {
    //FIXME - do we need to explicitily delete the KgramInfo? *(kit->second)~;
    allkeys.erase(kit->first);
  }
}


//-------------------------------------------------------------------------
// KeyTable methods
//-------------------------------------------------------------------------

// Iterate over all documents in the DocSet, or only those between startFile
// and endFile (if these params>=0), and add short kgram keys to the keytable
// passed in.
//
// maxKeysToCount currently ignored [FIXME/Simeon/2005-08-03]
// 
void DocSet::addToKeyTable(KeyTable& kt, int maxKeysToCount, int startFile, int endFile) {
  int i=0;	//number of documents in list
  for (DocInfoVector::iterator docit=docv.begin();docit!=docv.end();docit++) {
    i++;
    if (((startFile<0) || (i>=startFile)) && ((endFile<0) || (i<=endFile))) {
      docit->addToKeyTable(kt, maxKeysToCount);
      if (VERY_VERBOSE) {
        cout << "DocSet::addToKeyTable[" << i << "]: " << docit->filename << " (vv)" <<endl;
      } else if (VERBOSE && i%1000==0) {
        cout << "DocSet::addToKeyTable[" << i << "]: " << docit->filename << endl;
      } else if (i%10000==0) {
        cout << "DocSet::addToKeyTable[" << i << "]: " << docit->filename << endl;
        kt.writeStats(cout);
      }
    }
  }	
  // Write out stats again unless we already just did it
  if (i%10000!=0) kt.writeStats(cout);
}


//-------------------------------------------------------------------------
// Utility methods
//-------------------------------------------------------------------------

// Write filename to id table
//
ostream& operator<<(ostream& out, DocSet& docs)
{
  for (DocInfoVector::iterator divit = docs.docv.begin(); divit != docs.docv.end(); divit++) {
    out << divit->id << "\t" << divit->filename << endl;
  }
  return out;
}



