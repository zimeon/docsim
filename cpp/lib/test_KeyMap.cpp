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
  TestMore tm=TestMore(3);
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

}
