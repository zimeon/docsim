// DocPair object encapsulates information about a pair
// of documents and how they match
// Simeon Warner - 2005-08-04
// $Id: DocPair.cpp,v 1.3 2011-02-10 05:10:24 simeon Exp $

#include "definitions.h"
#include "DocPair.h"
#include <fstream>


DocPair::DocPair(docid i1, docid i2, int sk)
{
  id1=i1;
  id2=i2;
  sharedKeys=sk;
}

DocPair::~DocPair(void)
{
}


ostream& operator<<(ostream& out, DocPair& dp)
{
  out << dp.id1 << " " << dp.id2 << " " << dp.sharedKeys << endl;
  return(out);
}


ostream& operator<<(ostream& out, DocPairVector& dpv)
{
  for (DocPairVector::iterator it=dpv.begin(); it<dpv.end(); ++it) {
    out << *it;
  }
  return(out);
}
