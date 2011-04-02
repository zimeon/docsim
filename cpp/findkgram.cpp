// findkgram.cpp
// Find kgram in a normalized txt file
// Simeon Warner, 2005-
//
// $Id: findkgram.cpp,v 1.2 2011-03-03 14:20:24 simeon Exp $
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

  // Read options using standard code for all of DocSim programs
  readOptions(argc, argv, "f:k:K:svV", myname, "Look for kgrams matching either the kgram (-k) or kgram key (-K) specified, in the file given (-f).");

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
    kk=stringToKgramkey(keystr);
  } else {
    cerr << myname << ": must specify either -k or -K options" << endl;
    exit(1);
  }
  cout << myname << ": looking for key " << kgramkeyToString(kk) << endl;
      
  DocInfo doc;
  doc.filename=filename1.c_str();
  char* match=doc.findKgramInDoc(kk);
  if (match!=(char*)NULL) {
    cout << myname << ": Found kgram: '" << match << "'" << endl;
  } else {
    cout << myname << ": kgram not found in " << filename1 << endl;
  }
  
  return 0;
}
