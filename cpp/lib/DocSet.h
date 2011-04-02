// Information about the collection of documents to be processed
//

#ifndef __INC_DocSet
#define __INC_DocSet 1

#include "definitions.h"
#include "KeyTable.h"
#include "DocInfo.h"

typedef vector<DocInfo> DocInfoVector;

class DocSet
{
public:
  // DATA
  DocInfoVector docv; // array of DocInfo objects

  // METHODS
  DocSet(void);
  ~DocSet(void);
  DocInfo& operator[](docid id);
  void readFileList(const string filename, const string dataDir="");
  docid addFile(const string psvFile, const string dataDir="");
  int size() { return (int)docv.size(); }

  // Methods for dealing with a keymap 
  void getKeymap(keymap& allkeys, int maxKeysToCount, bool winnow=true, int startFile=-1, int endFile=-1);
  void stripCommon(keymap& keys, keymap& common, int numDupesToBeCommon);

  // Methods for dealing with a KeyTable
  void addToKeyTable(KeyTable& kt, int maxKeysToCount, int startFile=-1, int endFile=-1);  

  friend ostream& operator<<(ostream& out, DocSet& docv);

private:

};

#endif /* #ifndef __INC_DocSet */
