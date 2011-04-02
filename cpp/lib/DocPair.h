// Information about a document pair and their overlap.
//
// Also defines a vector type DocPairVector which is a set of
// document pairs.
//
// $Id: DocPair.h,v 1.2 2008-11-19 22:55:52 simeon Exp $

#ifndef __INC_DocPair
#define __INC_DocPair 1

#include "definitions.h"

class DocPair
{
public:
  // DATA
  docid id1;         // document id
  docid id2;         // document id
  int sharedKeys;

  // METHODS
  DocPair(docid i1, docid i2, int sk);
  ~DocPair(void);

  friend ostream& operator<<(ostream& out, DocPair& k);
  
};

typedef vector<DocPair> DocPairVector;
ostream& operator<<(ostream& out, DocPairVector& k);

#endif /* #ifndef __INC_DocPair  */
