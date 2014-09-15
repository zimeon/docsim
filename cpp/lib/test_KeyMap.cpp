// Test code for KeyMap
// Simeon Warner - 2014-08-16

#include "definitions.h"
#include "options.h"
#include "kgrams.h"
#include "KeyMap.h"
#include "TestMore.h"

#include <sstream>

string km_str(KeyMap &km)
{
  ostringstream os;
  os << km;
  return(os.str());
}


int main(int argc, char* argv[]) 
{
  VERY_VERBOSE = true;
  TestMore tm=TestMore(9);
  // Test building and display of KeyMap
  //
  KeyMap km1;
  tm.is( km_str(km1), "", "Empty Keymap" );

  KeyMap km2;
  tm.is( km_str(km2), "", "Empty Keymap" );
  KgramInfo ki1;
  ki1.addOccurrence(1);
  km2.insert(KeyMap::value_type(stringToKgramkey((char*)"0000000000000111"),&ki1));
  tm.is( km_str(km2), "0000000000000111  [1,1] 1\n", "One entry" );

  KgramInfo ki2;
  ki2.addOccurrence(1);
  ki2.addOccurrence(2);
  km2.insert(KeyMap::value_type(stringToKgramkey((char*)"0000000000000222"),&ki2));
  tm.contains( km_str(km2), "0000000000000111  [1,1] 1\n", "Two entries, contains original" );
  tm.contains( km_str(km2), "0000000000000222  [2,2] 1 2\n", "Two entries, contains extra" );

  ////////////////////////////////////////////////////////////////
  // Reading...
  struct tp {
    string inp;
    int size;
    bool good;
  };
  tp tests[5];
  tests[0].inp = "0000000000000111  [1,1] 1\n0000000000000222  [2,2] 1 2";
  tests[0].size = 2;
  tests[0].good = false;
  tests[1].inp = "0000000000000111  [1,1] 1\n0000000000000222  [2,2] 1 2\n";
  tests[1].size = 2;
  tests[1].good = false;
  for (int j=0; j<2; j++) {
    istringstream in(tests[j].inp);
    KeyMap kmin;
    in >> kmin;
    tm.is( (int)kmin.size(), tests[j].size, "size" );
    tm.is( (int)in.good(), (int)tests[j].good, "fh status" );
  }
}
