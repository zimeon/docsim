// KeyMap object
// Simeon Warner - 2005-08-24..

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


// Clean up memory on destruct, each element in the keymap has
// a KgramInfo object as the value. If code uses the keymap type
// directly then an explcit 
//
KeyMap::~KeyMap(void)
{
  // cerr << "Called KeyMap destructor" << endl;
  for (KeyMap::iterator kit=begin(); kit!=end(); kit++) { 
    delete kit->second;
  }
  clear();
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
  cout << "getCommonDocs: called on keymap with " << size() << " ids" << endl;
  // Find largest docid
  docid maxDocid=0;
  for (KeyMap::iterator kit=begin(); kit!=end(); kit++) {
    // kit->second is KgramInfo object, has ids[0..idsSize]
    KgramInfo* ki=kit->second;
    for (int j=0; j<(ki->idsSize); j++) {
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

  // Now run through complete keymap and increment count for each time
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
// Non-member utility functions to write and read 'keymap' objects which are
// thus inherited by KeyMaps.
//
// (might want to have a binary format for these if they are to be used
// with large objects)
//
// ASCII format of lines
//   kgramkey KgramInfo
// If there is no KgramInfo then the string '[null]' is used in its place
//
ostream& operator<<(ostream& out, keymap& keys)
{
  if (VERY_VERBOSE) cout << "keymap::operator<<: writing keymap with " << keys.size() << " entries" << endl;
  for (keymap::iterator kit=keys.begin(); kit!=keys.end(); kit++) {
    out << kgramkeyToString(kit->first) << ' ';
    if (kit->second==(KgramInfo*)NULL) {
      out << "[null]\n";
    } else {
      out << *(kit->second) << endl;
    }
  }
  return out;
}


istream& operator>>(istream& in, keymap& keys)
{
  if (VERY_VERBOSE) cout << "keymap::operator>>: reading keymap with " << keys.size() << " entries beforehand" << endl;
  char kstr[KGRAMKEYDIGITS];
  kgramkey key;
  while (in) {  
    KgramInfo* ki=new KgramInfo();
    in >> kstr >> *ki;
    key=stringToKgramkey(kstr);
    keys.insert(keymap::value_type(key,ki));
    //if (VERY_VERBOSE) cout << "keymap::operator>>: read: " << kgramkeyToString(key) << ki << endl;
  }
  if (VERY_VERBOSE) cout << "keymap::operator>>: read keymap, now has " << keys.size() << " entries" << endl;
  return in;
}


//===================================================================================
// Take two keymaps and filters out all keys that do not exist in the second keymap
//
void filterKeymap(keymap& ks, keymap& kf, keymap& kr)
{
  for (keymap::iterator kfit=kf.begin(); kfit!=kf.end(); kfit++) {
    //cout << "kfit: " << *kfit->second << endl;
    keymap::iterator ksit=ks.find(kfit->first);
    if (ksit!=ks.end()) {
      // This kgram is in both maps
      //cout << "ksit: " << *ksit->second << endl;
      KgramInfo* kip=new KgramInfo(*ksit->second);
      //cout << kip  << endl;
      kr.insert(keymap::value_type(ksit->first,kip));
    }
  }
}
