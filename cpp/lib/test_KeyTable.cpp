// Test code for KeyTable
// Simeon Warner - 2005-07-18
// $Id: test_KeyTable.cpp,v 1.6 2011-04-02 16:27:15 simeon Exp $

#include "definitions.h"
#include "kgrams.h"
#include "KeyMap.h"
#include "KeyTable.h"
#include "options.h"

int main(int argc, char* argv[]) 
{
  VERY_VERBOSE=true;

  // Test building and display of KeyTable
  //
  KeyTable k(20);
  cout << "Created KeyTable k, stats are:" << endl;
  k.writeStats(cout); 
  cout << "Table is:" << endl << k;
  //
  // Simple range checks
  cout << endl << "KeyTable.getDocids RANGE CHECKS" << endl;
  intv docids;
  cout << "  min=0" << endl;
  k.getDocids(docids,0);       //OK, min
  cout << "  max=1048575" << endl;
  k.getDocids(docids,1048575); //OK, max
  //kt.getDocids(docids,-1);      //one too small
  //kt.getDocids(docids,1048576); //one too big
  //
  // Now add 10 different elements, 
  for (int j=1; j<=10; j++) {
    k.addKey((kgramkey)j,j);
  }
  cout << "Added 10 non-overlapping elements, stats are:" << endl;
  k.writeStats(cout); 
  cout << "Table is:" << endl << k;
  //
  // Try to add dupe to element 1
  kgramkey ten=10;
  k.addKey(ten,10);
  cout << "Attempted to add same element again (10,10), stats are:" << endl;
  k.writeStats(cout); 
  cout << "Table is:" << endl << k;
  //
  // Now add 5 overlapping elements, 
  for (int j=1; j<=5; j++) {
    k.addKey((kgramkey)j,j+10);
  }
  cout << "Added 5 overlapping elements, stats are:" << endl;
  k.writeStats(cout); 
  cout << "Table is:" << endl << k;
  //
  // Try to add same element with same docid again (should be ignored)
  kgramkey five=5;
  k.addKey(five,15);
  cout << "Attempted to add same element again (5,15), stats are:" << endl;
  k.writeStats(cout); 
  cout << "Table is:" << endl << k;
  //
  // Now add 100 more overlaps to element 1, 
  kgramkey one=1;
  for (int j=1; j<=100; j++) {
    k.addKey(one,j+15);
  }
  cout << "Added 100 extra docs for key 1, stats are:" << endl;
  k.writeStats(cout); 
  cout << "Table is:" << endl << k;
  //
  // Try to add same element with same docid again (should be ignored)
  k.addKey(one,115);
  cout << "Attempted to add same element again (1,115), stats are:" << endl;
  k.writeStats(cout); 
  cout << "Table is:" << endl << k;
  //
  string outFile1="/tmp/test_KeyTable1.keytable";
  cout << "Writing KeyTable as single file to " << outFile1 << endl;
  ofstream ktout;
  ktout.open(outFile1.c_str(),ios_base::out);
  ktout << k;
  ktout.close();
  //
  string outFile2="/tmp/test_KeyTableM";
  cout << "Writing KeyTable as multi file with base " << outFile2 << endl;
  k.writeMultiFile(outFile2,1,50);
  //
  //================ Reading files =====================
  // 
  KeyTable kt(20);
  cout << "Reading KeyTable as single file from " << outFile1 << endl; 
  ifstream ktin;
  ktin.open(outFile1.c_str(),ios_base::in);
  ktin >> kt;
  ktin.close();
  cout << "Read KeyTable from " << outFile1 << endl << kt;
  // 
  KeyTable kt2(20);
  cout << "Reading KeyTable as multiple files with base " << outFile2 << endl; 
  kt2.readMultiFile(outFile2);
  cout << "Read KeyTable from " << outFile2 << endl << kt2;

  //
  // =============== Reading KeyTable back as KeyMap ==================
  //
  KeyTable dummyKT(20,false);

  KeyMap km;
  cout << "Reading KeyTable as KeyMap single file from " << outFile1 << endl;
  ifstream ktin2;
  ktin2.open(outFile1.c_str(),ios_base::in);
  if (!ktin2) {
    cerr << "test_KeyTable: failed to open " << outFile1 << endl;
  } 
  dummyKT.readTables123(ktin2, (indexhashset*)NULL, &km);
  ktin2.close();
  cout << "Got KeyMap from " << outFile1 << endl << km;

  km.clear();
  indexhashset indexes;
  indexes.insert(5);
  indexes.insert(6);
  indexes.insert(7);
  indexes.insert(8);
  cout << "Reading KeyTable as KeyMap single file from " << outFile1 << " with filter (5,6,7,8)" << endl;
  ifstream ktin3;
  ktin3.open(outFile1.c_str(),ios_base::in);
  if (!ktin3) {
    cerr << "test_KeyTable: failed to open " << outFile1 << endl;
  } 
  dummyKT.readTables123(ktin3, &indexes, &km);
  ktin3.close();
  cout << "Got KeyMap from " << outFile1 << " with filter (5,6,7,8)" << endl << km;

  cout << "Done." << endl;
}
