// Test code for KgramInfo.ccp/KgramInfo.h
// Simeon Warner - 2005-08-10
// $Id: test_KgramInfo.cpp,v 1.1 2005/08/10 21:08:31 simeon Exp $

#include "definitions.h"
#include "kgrams.h"
#include "KgramInfo.h"

#include <sstream>

int main(int argc, char* argv[]) 
{
  cout << "test_KgramInfo" << endl;
  ///////////////////////////////////////////////////////////////
  // Test building and display of KeyInfo
  //
  KgramInfo ki1(1);
  cout << "Created KgramInfo ki1, initial form: " << ki1 << endl;
  //
  ki1.addOccurrence(2);
  cout << "Added doc 2: " << ki1 << endl;
  //
  ki1.addOccurrence(2);
  cout << "Added doc 2 again: " << ki1 << endl;
  //
  ki1.addOccurrence(2);
  cout << "Added doc 2 again: " << ki1 << endl;
  //
  ki1.addOccurrence(3);
  cout << "Added doc 3: " << ki1 << endl;

  ////////////////////////////////////////////////////////////////
  // Creation and use of initially blank KgramInfo object
  KgramInfo ki2;
  cout << "Created blank KgramInfo ki2, initial form: " << ki2<< endl;
  //
  ki2.addOccurrence(10);
  cout << "Added doc 10: " << ki2 << endl;

  ////////////////////////////////////////////////////////////////
  // Reading...
  cout << endl << "Now to test reading..." << endl;

  string buf="[5,4] 1 2 3 4";
  istringstream in(buf);
  KgramInfo kin;
  in >> kin;
  cout << "Read '" << buf << "'" << endl;
  cout << "Got  '" << kin << "'" << endl;
  //
  cout << "Done." << endl;
}
