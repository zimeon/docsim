// $Id: kgrams.h,v 1.1 2007/04/23 23:47:08 simeon Exp $

#ifndef __INC_kgrams
#define __INC_kgrams 1

#include "definitions.h"
#include "kgrams.h"
#include "KgramInfo.h"
#include <algorithm>
#include <fstream>

kgramkey fingerprint(char* startch, char* endch);
string kgramkeyToString(U64 hash);
kgramkey stringToKgramkey(char* keystr);
kgramkey* findSmallestKgramkey(kgramkey* startkey, kgramkey* endkey, kgramkey* lastkey);
kgramkey* getKgrams(char* sentence, bool winnow=true);
char* findKgram(kgramkey& key, char* sentence);
int findWordsInKgrams(intv& spaces, intv& words, kgramkeyv& keystarts, keyhashset& keys, char* sentence);
int findSpaces(intv& spaces, char* sentence);

ostream& operator<<(ostream& out, kgramkey& k);
ostream& operator<<(ostream& out, kgramkeyv& keys);
ostream& operator<<(ostream& out, keyhashset& keys);
void readKeyhashset(const char* filename, keyhashset& keys);

#endif //__INC_kgrams
