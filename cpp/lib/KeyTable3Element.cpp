// Class for use as element in table3 of KeyTable. Designed to mimic
// a STL vector<int> but to be efficient for small lists while still
// growable to large lists. Doing this because using a vector of 
// vector<int> for table3 ended up taking up too much memory.
//
// $Id: KeyTable3Element.cpp,v 1.2 2011-03-03 14:20:24 simeon Exp $

#include "KeyTable3Element.h"
#include <stdlib.h>  // for exit()
#include <iostream>
#include <fstream>


KeyTable3Element::KeyTable3Element()
{
  max = 3;
  x = new int[max+1];
  last = -1;
}


KeyTable3Element::KeyTable3Element(int n)
{
  max = n;
  x = new int[max+1];
  last = -1;
}


KeyTable3Element::~KeyTable3Element(void)
{
  //should self-clean OK
}


void KeyTable3Element::push_back(int i)
{
  // Do we need to grow?
  if (last+1>=max) {
    // New space
    int max_new=(max+1)*2;
    int* xnew = new int[max_new+1];
    // Copy
    for (int j=0; j<=max; j++) {
      xnew[j]=x[j];
    }
    // Switch (destroy old table)
    delete x; 
    x=xnew;
    max=max_new;
  }

  // Add in new element
  x[++last]=i;
}


// Last element, will die if attempt to read from empty
//
int KeyTable3Element::back(void)
{
  if (last<0) {
    std::cerr << "KeyTable3Element::back: Attempt to read from empty array" << std::endl;
    exit(1);
  }
  return(x[last]);
}


// Number of entries. We count from 0 so add
// one to index to get number.
//
int KeyTable3Element::size(void)
{
  return(last+1);
}

int KeyTable3Element::capacity(void)
{
  return(max);
}

int KeyTable3Element::operator[](int i)
{
  return(x[i]);
}

int KeyTable3Element::size_in_bytes(void)
{
  return(sizeof(int)*(max+3));
}


int* KeyTable3Element::begin(void)
{
  return(x);
}

// Last index (-1 for no entries)
//
int* KeyTable3Element::end(void)
{
  return(x+last+1);
}



// Write out space separated
//
std::ostream& operator<<(std::ostream& out, KeyTable3Element& k) {
  //out << "operator<<:: begin=" << k.begin() << " end=" << k.end() << std::endl;
  for (KeyTable3Element::iterator kti=k.begin();
       kti!=k.end();
       kti++) {
    if (kti!=k.begin()) {
      out << " ";
    }
    out << *kti;
  }
  return(out);
}

