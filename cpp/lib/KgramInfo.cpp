// Information about a kgram (use, list of docids...)
// Simeon Warner - 2005-07...

#include "definitions.h"
#include "KgramInfo.h"

#define INITIAL_SIZE 2
#define MAX_SIZE 60000


KgramInfo::KgramInfo(void)
{
  idsSize=INITIAL_SIZE;
  ids=new docid[idsSize];
  numIds=0;
  occurrences=0;
  //cerr << "KgramInfo::KgramInfo(void) -- attempt to create blank KgramInfo object, not allowed" << endl;
  //exit(3);
}


KgramInfo::KgramInfo(docid id)
{
  idsSize=INITIAL_SIZE;
  ids=new docid[idsSize];
  numIds=1;
  ids[0]=id;
  occurrences=1;
  //cout << "KgramInfo::new[" << (ptr_to_int)this << "] id=" << id << " numIds=" << numIds << " occurrences=" << occurrences << "\n";
}


KgramInfo::KgramInfo(intv& idv)
{
  idsSize=idv.size();
  ids=new docid[idsSize];
  numIds=idsSize;
  for (int j=0; j<numIds; j++) ids[j]=idv[j];
  occurrences=idsSize; // don't actually have data on real occurrences so fudge to number of docs
}


// Initialize as simple copy of existing KgramInfo object
// 
KgramInfo::KgramInfo(KgramInfo* ki)
{
  idsSize=ki->idsSize;
  ids=new docid[idsSize];
  numIds=ki->numIds;
  for (int j=0; j<numIds; j++) ids[j]=ki->ids[j];
  occurrences=ki->occurrences;
}


// Destructor: Free memory in ids array
//
KgramInfo::~KgramInfo(void)
{
  delete[] ids;
}


// Don't ever plan to use this so make it exit with a warning just in case
// I write code that accidentally creates extra copies of these objects.
//
KgramInfo& KgramInfo::operator=(const KgramInfo& ki)
{
  cerr << "KgramInfo::operator= from " << (ptr_to_int)&ki << "\n";
  exit(2);
}


// Here we recklessly use the fact that we add documents in order of docid
// We can thus check the last entry to see if the docid is the same
//
// maxDupesToCount is currently unused.
//
void KgramInfo::addOccurrence(docid id, int maxDupesToCount)
{
  docid lastId=0; // less than smallest docid (==1)
  if (numIds>0) {
    lastId=ids[numIds-1];
    //cout << "KgramInfo::addOccurrence[" << (ptr_to_int)this << "] add " << id << "\n";
    if (id<lastId) {
      cerr << "KgramInfo::addOccurrence: ERROR -- attempt to use with docs out of order!\n";
      cerr << "KgramInfo::addOccurrence: ERROR " << (ptr_to_int)this << " id="<<id<<" numIds="<<numIds<<" lastId=ids[numIds-1]="<<lastId<<"\n";
      exit(1);
    }
  }
  if (id>lastId) {
    // Got new id
    if (numIds==idsSize) growIds();
    ids[numIds++]=id;
    //cout << "KgramInfo::addOccurrence: added id=" << id << " numIds=" << numIds << " (lastId=" << lastId << ")\n";
  } else {
    //cout << "KgramInfo::addOccurrence: same id=" << id << ", incrementing occurrences to " << (occurrences+1) << "\n";
  }
  // Add extra occurrence if new or not
  occurrences++;
}


int KgramInfo::size(void)
{
  return(numIds);
}

// Grow the ids array. Relies only on the maximum size and not the number
// of ids currently in use (numIds). Blindly copies the whole array.
//
void KgramInfo::growIds(void)
{
  docid *oldids = ids;
  int oldidsSize = idsSize;

  //cout << "KgramInfo::growIds[" << (ptr_to_int)this << "] was size " << idsSize << "\n";
  if ((int)idsSize>(MAX_SIZE/2)) {
    cerr << "KgramInfo::growIds: can't grow ids array from size " << idsSize << "\n";
    exit(1);
  }

  idsSize+=idsSize;
  ids = new docid[idsSize];
  for (int j=0; j<oldidsSize; j++) {
    ids[j]=oldids[j];
  }
    
  delete[] oldids;  
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Utility functions

// Format of KgramInfo object is
//   [#occurrences,#docs] #docid1 #docid2..
//
ostream& operator<<(ostream& out, KgramInfo& ki)
{
  out << "[" << ki.occurrences << "," << ki.numIds << "]";
  if (ki.ids!=(docid*)NULL) {
    for (int j=0; j<ki.numIds; j++) {
      out << " " << ki.ids[j];    
    }
  } else {
    out << "ERROR_no_ids_array";
  }
  return out;
}


istream& operator>>(istream& in, KgramInfo& ki)
{
  int ch;
  while (in && ((ch=in.get())!='[')) { /* Skip anything to [ */ }
  in >> ki.occurrences;
  if ((ch=in.get())!=',') { // next thing must be comma
    cerr << "operator>> for KgramInfo, expected comma, got character " << ch << ", bad input" << endl;
    in.clear(ios::badbit|in.rdstate()); // set stream bad
  }
  in >> ki.numIds;
  if ((ch=in.get())!=']') { // next thing must be closing square bracket
    cerr << "operator>> for KgramInfo, expected ], got character " << ch << ", bad input" << endl;
    in.clear(ios::badbit|in.rdstate()); // set stream bad
  }
  // now expect numIds space separated numbers
  for (int j=0; j<ki.numIds; j++) {
    if (j==ki.idsSize) ki.growIds();
    docid did;
    in >> did;
    ki.ids[j]=did;
  }
  while (in && ((ch=in.get())!='\n')) { 
    // Skip spaces to end of line
    if (ch!=' ') {
      cerr << "operator>> for KgramInfo, permit trailing space, got char " << ch << ", bad input" << endl;
      in.clear(ios::badbit|in.rdstate()); // set stream bad
    }
  }
  if (in && (ch=in.get())) { in.putback(ch); } // read ahead to set in false if next to end
  return in;
}
