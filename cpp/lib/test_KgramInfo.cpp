// Test code for KgramInfo.ccp/KgramInfo.h
// Simeon Warner - 2005-08-10

#include "definitions.h"
#include "kgrams.h"
#include "KgramInfo.h"
#include "TestMore.h"

#include <sstream>

string ki_str(KgramInfo &ki)
{
  ostringstream os;
  os << ki;
  return(os.str());
}

int main(int argc, char* argv[]) 
{
  TestMore tm=TestMore(17);

  ///////////////////////////////////////////////////////////////
  // Test building and display of KgramInfo
  //
  KgramInfo ki1(1);
  tm.is( ki_str(ki1), "[1,1] 1",
         "Created KgramInfo ki1, with doc 1" );
  ki1.addOccurrence(2);
  tm.is( ki_str(ki1), "[2,2] 1 2",
         "Added doc 2" );
  ki1.addOccurrence(2);
  tm.is( ki_str(ki1), "[3,2] 1 2",
         "Added doc 2 again" );
  ki1.addOccurrence(2);
  tm.is( ki_str(ki1), "[4,2] 1 2",
         "Added doc 2 again" );
  ki1.addOccurrence(3);
  tm.is( ki_str(ki1), "[5,3] 1 2 3",
         "Added doc 3" );

  ////////////////////////////////////////////////////////////////
  // Creation and use of initially blank KgramInfo object
  KgramInfo ki2;
  tm.is( ki_str(ki2), "[0,0]",
         "Created KgramInfo ki2, initial empty form" );
  ki2.addOccurrence(10);
  tm.is( ki_str(ki2), "[1,1] 10",
         "Added doc 10" );

  ////////////////////////////////////////////////////////////////
  // Reading...
  struct tp {
    string inp;
    string out;
  };
  tp tests[5];
  tests[0].inp = "[0,0]";
  tests[0].out = "[0,0]";
  tests[1].inp = "[0,0]\n";
  tests[1].out = "[0,0]";
  tests[2].inp = "[0,0]       \n";
  tests[2].out = "[0,0]";
  tests[3].inp = "[0,0]\nnext line";
  tests[3].out = "[0,0]";
  tests[4].inp = "[5,4] 1 2 3 4";
  tests[4].out = "[5,4] 1 2 3 4";
  for (int j=0; j<5; j++) {
    istringstream in(tests[j].inp);
    KgramInfo kin;
    in >> kin;
    tm.is( ki_str(kin), tests[j].out, "Read test" );
    if (j==3) {
      tm.ok( in.good(), "Stream good" );
    } else {
      tm.ok( not in.good(), "Stream finished" );
    }
  }
}
