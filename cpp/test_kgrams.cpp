// Test code for kgram.ccp/kgram.h
// Simeon Warner - 2005-07-18
// $Id: test_kgrams.cpp,v 1.1 2005/07/19 21:53:18 simeon Exp $

#include "definitions.h"
#include "kgrams.h"

int main(int argc, char* argv[]) 
{
  // Test conversion of keys to and from strings
  //
  char* key1="01fda04908cdbef3";
  cout << "key to start:        " << key1 << endl;
  kgramkey kk=stringToKgramkey(key1);
  string key2=kgramkeyToString(kk);
  cout << "key after roundtrip: " << key2 << endl;
  //
}
