// Test code for KeyTable3Element
// Simeon Warner - 2008-11-19
// $Id: test_KeyTable3Element.cpp,v 1.1 2011-03-03 14:10:58 simeon Exp $

using namespace std;
#include "KeyTable3Element.h"
#include <iostream>

int main(int argc, char* argv[]) 
{
  // Test building and display of KeyTable
  //
  KeyTable3Element k;
  cout << "Created KeyTable3Element k, size=" << k.size() << " capacity=" << k.capacity() << " size_in_bytes=" << k.size_in_bytes() << endl;
  cout << k;

  k.push_back(123);
  k.push_back(456);
  cout << "After two additions:" << endl << k << endl;
  cout << "back=" << k.back() << " size=" << k.size() << " capacity=" << k.capacity() << " size_in_bytes=" << k.size_in_bytes() << endl;;

  k.push_back(9123);
  k.push_back(9456);
  cout << "After two more additions:" << endl << k << endl;
  cout << "back=" << k.back() << " size=" << k.size() << " capacity=" << k.capacity() << " size_in_bytes=" << k.size_in_bytes() << endl;;

  for (int j=0; j<1000; j++) {
    k.push_back(j);
  }
  cout << "After 1000 more additions:" << endl << k << endl;
  cout << "back=" << k.back() << " size=" << k.size() << " capacity=" << k.capacity() << " size_in_bytes=" << k.size_in_bytes() << endl;;

  cout << "Element 0 = " << k[0] << endl;
  cout << "Element 99 = " << k[99] << endl;
  cout << "Element 999 = " << k[999] << endl;

}
