// Wrapper for a keymap hash to add some bookkeeping and to obscure
// some of the messy innards.
// Simeon Warner - 2005-08-24...
// $Id: KeyMap.h,v 1.1 2007/04/23 23:47:08 simeon Exp $

#ifndef __INC_KeyMap
#define __INC_KeyMap 1

#include "definitions.h"
#include "kgrams.h"
#include "KgramInfo.h"
#include "DocPair.h"

typedef hash_map<kgramkey,KgramInfo*> keymap;

ostream& operator<<(ostream& out, keymap& keys);
istream& operator>>(istream& in, keymap& keys);
void filterKeymap(keymap& ks, keymap& kf, keymap& kr);
void getCommonDocs(keymap& keys, DocPairVector& dpv, int n, docid id2=9999999);

class KeyMap
{
public:
  // DATA
  keymap keys;

  // METHODS
  KeyMap(void);
  ~KeyMap(void);

  friend ostream& operator<<(ostream& out, KeyMap& k);
  friend istream& operator>>(istream& in, KeyMap& k);

};

inline void filterKeymap(KeyMap& ks, KeyMap& kf, KeyMap& kr) { filterKeymap(ks.keys,kf.keys,kr.keys); }


#endif /* #ifndef __INC_KeyMap */
