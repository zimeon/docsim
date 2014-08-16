// kgramkey.cpp
// Very simple program to accept kgram input and write out key
// Simeon Warner, 2011-03-09-
//
#include "definitions.h"
#include "options.h"
#include "kgrams.h"
#include <string.h> // for strncpy()
#include <unistd.h> // assume GNU getopt
#include <fstream>

// Globals defined in definitions.h and Options.h
const string myname="kgramkey";
#define MAX_KGRAM_LENGTH 200

int main(int argc, char* argv[])
{
  VERBOSE=0;
  VERY_VERBOSE=0;

  // bitsInKeyTable will be 0 unless -b option specifies a number of bits to compare with
  bitsInKeyTable=0;
  // Read options using standard code for all of DocSim programs
  readOptions(argc, argv, "b:K:vV", myname, "Take a single kgram (-K) and write out the key. No checks are made on whether the input kgram is in fact the correct number of words, it is treated as a string and hashed using the algorithm used for kgrams.");

  // Just look for one kgram specified on the command line
  //
  if (kgram.length()<=0) {
    cerr << myname << ": Must specify kgram with -K option" << endl;
    exit(1);
  } else if (kgram.length()>MAX_KGRAM_LENGTH) {
    cerr << myname << ": kgram is too long (" << kgram.length()
           << " chars exceeds " << MAX_KGRAM_LENGTH << " limit)" << endl;
    exit(1);
  }

  // Lenth OK, get on with it  
  char kgramstr[MAX_KGRAM_LENGTH+1];
  (void)strncpy(kgramstr,kgram.c_str(),MAX_KGRAM_LENGTH+1);
  if (VERBOSE) {
    cout << myname << ": fingerprinting  '" << kgram << "'" << endl;
  }
  kgramkey kk=fingerprint(kgramstr,kgramstr+kgram.length()-1);

  if (bitsInKeyTable==0) {
    cout << myname << ": key " << kgramkeyToString(kk) << endl;
  } else {
    if (VERBOSE) {
      cout << myname << ": full key " << kgramkeyToString(kk) << endl;
    }
    // Build so that mask is 0 or a set of bits 1's in the low order bits
    kgramkey mask=0;
    if (bitsInKeyTable>32) {
      cerr << "Error - max bits in KeyTable format is 32, can't do " << bitsInKeyTable << " bits" << endl;
      exit(2);
    } else {
      for (int j=0; j<bitsInKeyTable; j++) {
	mask = (mask<<1 | 1);
      }
    }
    char buf[9];
    // Currently use 8 char keys in KeyTable code so always write as that
    sprintf(buf,"%08x",(U32)(kk&mask));
    cout << myname << ": " << bitsInKeyTable << " bit key " << buf << endl; 
  }
  return 0;
}
