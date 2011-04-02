// Information about one document
//

#ifndef __INC_DocInfo
#define __INC_DocInfo 1

#include "definitions.h"
#include "KgramInfo.h"
#include "KeyTable.h"
#include "MarkedDoc.h"

class DocInfo
{
public:
  // DATA
  string filename;  // name of psv file
  docid id;         // document id
  
  // METHODS
  DocInfo(const string& fn="", const docid i=0);
  ~DocInfo(void);

  // building and using keymaps
  void addToKeymap(keymap& keys, int maxDupesToCount=-1, bool winnow=true);
  void addToKeymap(istream& in, keymap& keys, int maxDupesToCount=-1, bool winnow=true);
  char* findKgramInDoc(kgramkey key);
  void markupDoc(ostream& out, keyhashset& keys);
  void markupCompleteDoc(MarkedDoc& mud, keyhashset& keys);
  
  // building and using KeyTables
  void addToKeyTable(KeyTable& k, int maxDupesToCount);
};

#endif /* #ifndef __INC_DocInfo */
