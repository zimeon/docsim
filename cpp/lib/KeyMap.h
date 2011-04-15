// Wrapper class inhertied from a keymap unordered_map to add some bookkeeping 
// and to obscure some of the messy innards. In particular, use of KeyMap as 
// opposed to keymap will handle memory cleanup automatically with object
// destruction (use of keymap requires manual cleanup).
//
// Simeon Warner - 2005-08-24...
// 2011-04 - Handle cleanup

#ifndef __INC_KeyMap
#define __INC_KeyMap 1

#include "definitions.h"
#include "kgrams.h"
#include "KgramInfo.h"
#include "DocPair.h"

#ifdef __NO_TR1__
  typedef hash_map<kgramkey,KgramInfo*> keymap;
#else
  typedef std::tr1::unordered_map<kgramkey,KgramInfo*> keymap;
#endif

ostream& operator<<(ostream& out, keymap& keys);
istream& operator>>(istream& in, keymap& keys);
void eraseKeymap(keymap& keys);
void filterKeymap(keymap& ks, keymap& kf, keymap& kr);
//void getCommonDocs(keymap& keys, DocPairVector& dpv, int n, docid id2=9999999);

class KeyMap: public keymap {
public:
  // METHODS
  KeyMap(void);
  ~KeyMap(void);
  void getCommonDocs(DocPairVector& dpv, int n, docid id2=9999999);

};

inline void filterKeymap(KeyMap& ks, KeyMap& kf, KeyMap& kr) { filterKeymap(ks,kf,kr); }

#endif /* #ifndef __INC_KeyMap */
