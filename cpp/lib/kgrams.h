// $Id: kgrams.h,v 1.2 2011-03-10 00:03:59 simeon Exp $

#ifndef __INC_kgrams
#define __INC_kgrams 1

#include "definitions.h"
#include <algorithm>
#include <fstream>

kgramkey fingerprint(char* startch, char* endch);
string kgramkeyToString(U64 hash);
kgramkey stringToKgramkey(char* keystr, int chars=0);
kgramkey* findSmallestKgramkey(kgramkey* startkey, kgramkey* endkey, kgramkey* lastkey);
kgramkey* getKgrams(char* sentence, bool winnow=true);
char* findKgram(kgramkey& key, char* sentence);
char* findKgramWithMask(kgramkey& key, kgramkey mask, char* sentence);
int findWordsInKgrams(intv& spaces, intv& words, kgramkeyv& keystarts, keyhashset& keys, char* sentence);
int findSpaces(intv& spaces, char* sentence);

ostream& operator<<(ostream& out, kgramkey& k);
ostream& operator<<(ostream& out, kgramkeyv& keys);
ostream& operator<<(ostream& out, keyhashset& keys);
void readKeyhashset(const char* filename, keyhashset& keys);

#endif //__INC_kgrams
