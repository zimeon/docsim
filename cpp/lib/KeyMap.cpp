// KeyMap object
// Simeon Warner - 2005-08-24..
//
// A KeyMap is a mapping from key values to KgramInfo objects implemented
// as a hash (see header file for basis on STL hash). Access is provided
// through either an iterator (KeyMap::iterator) or the find(...) method. 
// Extra entries are added with insert(KeyMap::value_type(key,kgraminfo)).
//
// Each KgramInfo value has a list of document ids (in ki.ids[]) and some
// counting information.

#include "definitions.h"
#include "options.h"
#include "kgrams.h"
#include "KgramInfo.h"
#include "DocPair.h"
#include "KeyMap.h"
#include <algorithm>

KeyMap::KeyMap()
{
}


// Clean up memory on destruct, each element in the KeyMap has
// a KgramInfo object as the value.
//
KeyMap::~KeyMap(void)
{
  if (VERY_VERBOSE) cerr << "Called KeyMap destructor" << endl;
  for (KeyMap::iterator kit=begin(); kit!=end(); kit++) { 
    //if (kit->second!=(KgramInfo*)NULL) {
    //  delete kit->second;
    //}
  }
  
}


// Find all the documents in this KeyMap that occur more than n times. 
// Add the results to the DocPairVector dpv.
//
// id2 is passed in simply to output a set of DocPair values for all the
// documents found where id2 is the second id of the DocPair. This supports
// use when comparing id2 against a set of documents and pulling out those
// with overlap > n
// 
void KeyMap::getCommonDocs(DocPairVector& dpv, int n, docid id2)
{
  cout << "getCommonDocs: called on KeyMap with " << size() << " keys, threshold=" << n << endl;
  // Find largest docid
  docid maxDocid=0;
  for (KeyMap::iterator kit=begin(); kit!=end(); kit++) {
    // kit->second is KgramInfo object, has ids[0..idsSize]
    KgramInfo* ki=kit->second;
    for (int j=0; j<(ki->size()); j++) {
      if (ki->ids[j]>maxDocid) maxDocid=ki->ids[j];
    }
  }
  //cout << "getCommonDocs: maxDocid=" << maxDocid << endl;

  int* overlap=new int[maxDocid+1];
  if (overlap==(int*)NULL) {
    cerr << "getCommonDocs: Error - failure to allocate overlap[" << (maxDocid+1) << "] \arrary" << endl;
    exit(4);
  }
  
  // zero array to start
  for (docid j=1; j<=maxDocid; j++) {
    overlap[j]=0;
  }

  // Now run through complete KeyMap and increment count for each time
  // a docid appears
  for (KeyMap::iterator kit=begin(); kit!=end(); kit++) {
    // kit->second is KgramInfo object, has ids[0..idsSize]
    KgramInfo* ki=kit->second;
    for (int j=0; j<(ki->idsSize); j++) {
      overlap[ki->ids[j]]++;
    }
  }

  // Got through an add docids for which did[docid]>n to docids
  for (docid j=1; j<=maxDocid; j++) {
    if (overlap[j]>n) {
      DocPair dp(j,id2,overlap[j]);
      dpv.push_back(dp);
      //cerr << "getCommonDocs: Added " << dp << endl;
    }
  }

  delete[] overlap;
}

//===================================================================================
//
// Non-member utility functions to write and read KeyMap objects.
//
// (might want to have a binary format for these if they are to be used
// with large objects)
//
// ASCII format of lines
//   kgramkey KgramInfo
// If there is no KgramInfo then the string '[null]' is used in its place
//
ostream& operator<<(ostream& out, KeyMap& keys)
{
  if (VERY_VERBOSE) cout << "KeyMap::operator<<: writing KeyMap with " << keys.size() << " entries" << endl;
  for (KeyMap::iterator kit=keys.begin(); kit!=keys.end(); kit++) {
    out << kgramkeyToString(kit->first) << ' ';
    if (kit->second==(KgramInfo*)NULL) {
      out << "[null]\n";
    } else {
      out << *(kit->second) << endl;
    }
  }
  return out;
}


istream& operator>>(istream& in, KeyMap& keys)
{
  if (VERY_VERBOSE) cout << "KeyMap::operator>>: reading KeyMap with " << keys.size() << " entries beforehand" << endl;
  char kstr[KGRAMKEYDIGITS];
  kgramkey key;
  while (in.good()) {  
    KgramInfo ki=new KgramInfo();
    in >> kstr >> ki;
    key=stringToKgramkey(kstr);
    keys.insert(KeyMap::value_type(key,&ki));
    if (VERY_VERBOSE) cout << "KeyMap::operator>>: read: " << kgramkeyToString(key) << ki << endl;
  }
  if (VERY_VERBOSE) cout << "KeyMap::operator>>: read KeyMap, now has " << keys.size() << " entries" << endl;
  return in;
}


//===================================================================================
// Takes two KeyMaps (ks and kf) and add all shared to the a third (kr).
//
// Implememted by interating over all keys in the first KeyMap and searching for the
// key in the second. Adds to third (kr) if found. In normal use we expect kr to be 
// empty when called but this need not be the case.
//
void filterKeyMap(KeyMap& ks, KeyMap& kf, KeyMap& kr)
{
  for (KeyMap::iterator kfit=kf.begin(); kfit!=kf.end(); kfit++) {
    //cout << "kfit: " << *kfit->second << endl;
    KeyMap::iterator ksit=ks.find(kfit->first);
    if (ksit!=ks.end()) {
      // This kgram is in both maps
      //cout << "ksit: " << *ksit->second << endl;
      KgramInfo* kip=new KgramInfo(ksit->second);
      //cout << kip  << endl;
      kr.insert(KeyMap::value_type(ksit->first,kip));
    }
  }
}
