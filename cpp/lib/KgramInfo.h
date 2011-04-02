#ifndef __INC_KgramInfo
#define __INC_KgramInfo 1

#include "definitions.h"
#include "kgrams.h"
#include <algorithm>
#include <fstream>

class KgramInfo
{
public:
  // DATA
  int occurrences;    // total # of occurrences (counted multiply per document)
  U16 numIds;         // number of ids in use (# of documents kgram occurs in (counted once per document))
  U16 idsSize;        // size of ids array
  docid *ids;         // array of document ids
 
  // METHODS
  KgramInfo(void);
  KgramInfo(docid id);
  KgramInfo(intv& idv);
  KgramInfo(KgramInfo* ki);
  KgramInfo& operator=(const KgramInfo& ki);
  ~KgramInfo(void);
  void addOccurrence(docid id, int maxDupesToCount=-1);
  int size(void);
  void growIds(void);

  friend ostream& operator<<(ostream& out, KgramInfo& ki);
  friend istream& operator>>(istream& in, KgramInfo& ki);
};

#endif //__INC_KgramInfo
