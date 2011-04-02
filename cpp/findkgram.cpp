// findkgram.cpp
// Find kgram in a normalized txt file
// Simeon Warner, 2005-
//
// $Id: findkgram.cpp,v 1.3 2011-03-10 00:01:39 simeon Exp $
//
#include "definitions.h"
#include "options.h"
#include "Logger.h"
#include "DocSet.h"
#include "DocInfo.h"
#include "kgrams.h"
#include "files.h"
#include <string.h> // for strncpy()
#include <unistd.h> // assume GNU getopt
#include <fstream>

// Globals defined in definitions.h and Options.h
const string myname="findkgram";
#define MAX_KGRAM_LENGTH 200

int main(int argc, char* argv[])
{
  VERBOSE=0;
  VERY_VERBOSE=0;

  // bitsInKeyTable will be 0 unless -b option specifies a number of bits to compare with
  bitsInKeyTable=0;
  // Read options using standard code for all of DocSim programs
  readOptions(argc, argv, "b:f:k:K:svV", myname, "Look for kgrams matching either the kgram (-k) or kgram key (-K) specified, in the file given (-f). Optional -b parameter specifies how many bits should be used for the comparison, if matches have been found in a KeyTable then this would usually be the number of bits used in the KeyTable.");

  // Just look for one kgramkey specified on the command line
  //
  kgramkey kk;
  if (kgram.length()>0) {
    if (kgram.length()>MAX_KGRAM_LENGTH) {
      cerr << myname << ": kgram is too long (" << kgram.length()
           << " chars exceeds " << MAX_KGRAM_LENGTH << " limit)" << endl;
      exit(1);
    }
    char kgramstr[MAX_KGRAM_LENGTH+1];
    (void)strncpy(kgramstr,kgram.c_str(),MAX_KGRAM_LENGTH+1);
    cout << myname << ": fingerprinting  '" << kgram << "'" << endl;
    kk=fingerprint(kgramstr,kgramstr+kgram.length()-1);
  } else if (key.length()>0) {
    char keystr[18];
    (void)strncpy(keystr,key.c_str(),18);
    //  char* key1="01fda04908cdbef3";
    kk=stringToKgramkey(keystr, (bitsInKeyTable+3)/4 );
  } else {
    cerr << myname << ": must specify either -k or -K options" << endl;
    exit(1);
  }
  cout << myname << ": looking for key " << kgramkeyToString(kk) << endl;
      
  DocInfo doc;
  doc.filename=filename1.c_str();
  char* match=doc.findKgramInDoc(kk,bitsInKeyTable);
  if (match!=(char*)NULL) {
    cout << myname << ": Found kgram: '" << match << "'" << endl;
  } else {
    cout << myname << ": kgram not found in " << filename1 << endl;
  }
  
  return 0;
}
