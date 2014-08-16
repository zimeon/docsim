// Test code for KeyMap
// Simeon Warner - 2014-08-16

#include "definitions.h"
#include "options.h"
#include "kgrams.h"
#include "KeyMap.h"
#include "test_more.h"

#include <sstream>

string km_str(KeyMap &km)
{
  ostringstream os;
  os << km;
  return(os.str());
}


int main(int argc, char* argv[]) 
{
  plan(2);
  // Test building and display of KeyMap
  //
  KeyMap km;
  is( km_str(km), "", "Empty Keymap" );
  KgramInfo ki1;
  ki1.addOccurrence(1);
  km.insert(KeyMap::value_type(stringToKgramkey((char*)"0000000000000111"),&ki1));
  is( km_str(km), "0000000000000111  [1,1] 1\n", "One entry" );
  done_testing();
}
