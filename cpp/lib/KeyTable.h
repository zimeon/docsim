// A lookup table of short keys to document ids. Optimized for the case where 
// most keys point to zero or one document id. 
// Simeon Warner - 2005-08-03...
// $Id: KeyTable.h,v 1.4 2011-02-09 21:07:14 simeon Exp $

#ifndef __INC_KeyTable
#define __INC_KeyTable 1

#include "definitions.h"
#include "kgrams.h"
#include "KeyMap.h"
#include "DocPair.h"
#include "KeyTable3Element.h"

typedef vector<KeyTable3Element> table3_type;

class KeyTable
{
public:
  // SETUP
  int KEY_BITS;    // number of bits used in key
  int KEY_DIGITS;  // number of hex digits use to read/write key
  char* KEY_FMT;   // printf format for KEY_DIGITS hex digits
  int MAX_INDEX;   // largest index value, usually same as TABLE1_SIZE-1
  int EMPTY;       // value used to mark empty spaces in table1
  // DATA
  int TABLE1_SIZE;
  int* table1;
  int TABLE2_SIZE;
  int* table2;
  int table2_size;
  table3_type table3;
  int maxDocid;

  // METHODS
  KeyTable(int bits, bool dummy=false, int bitmask=0, int bitmaskMatch=0);
  ~KeyTable(void);
  void growTable2(void);
  void dropTable1(void);
  void addKey(kgramkey& key, int docid);
  void addKey(int i, int docid);
  int addKeyTable2(int i, int docid);
  int addKeyTable3(int i, int docid);
  //intv& operator[](int i);
  void getDocids(intv& docids, int i);

  void getOverlapIds(intv& docids, int n);
  void getOverlapDocs(DocPairVector& docpairs, intv& docids, int n);
  void getOverlapKeys(keymap& kms, keymap& kmd);
  void getOverlapKeys(intv& indexes, keymap& kmd);
  void keysToIndexes(keymap& km, intv& indexes);
  
  void writeStats(ostream& out);
  int writeTables123(ostream& out, int* positionPtr=(int*)NULL, long int bytes=-1);
  int writeTables23(ostream& out, int* postionPtr=(int*)NULL, long int bytes=-1);
  int writeMultiFile(string& baseName, bool allTables=1, long int maxFileSize=MAX_FILE_SIZE);
  void writeIndexes(ostream& out, intv& indexes);

  void setPruneAbove(int p);
  void noPrune(void);
  int readTables123(istream& in, intv* filterKeys=(intv*)NULL, keymap* km=(keymap*)NULL);
  int readTables23(istream& in, intv* filterKeys=(intv*)NULL, keymap* km=(keymap*)NULL);
  int readMultiFile(string& baseName, intv* filterKeys=(intv*)NULL, keymap* km=(keymap*)NULL);

  friend ostream& operator<<(ostream& out, KeyTable& k);
  friend istream& operator>>(istream& in, KeyTable& k);

private:
  bool readDocidList(istream& in, intv& docids);
  int stringToIndex(char* keystr);

  int SELECT_MASK;
  int SELECT_MATCH;
  int numDocidsInRead;
  
  int pruneAbove;

};

#endif /* #ifndef __INC_KeyTable */
