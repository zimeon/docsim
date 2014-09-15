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
using namespace std;

#ifdef __NO_TR1__
  typedef hash_map<kgramkey,KgramInfo*> keymap;
#else
  typedef std::tr1::unordered_map<kgramkey,KgramInfo*> keymap;
#endif

class KeyMap: public keymap {
public:
  // METHODS
  KeyMap(void);
  ~KeyMap(void);
  void getCommonDocs(DocPairVector& dpv, int n, docid id2=9999999);
};

ostream& operator<<(ostream& out, KeyMap& keys);
istream& operator>>(istream& in, KeyMap& keys);
void filterKeyMap(KeyMap& ks, KeyMap& kf, KeyMap& kr);

#endif /* #ifndef __INC_KeyMap */
