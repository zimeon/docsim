// Class for use as element in table3 of KeyTable. Designed to mimic
// a STL vector<int> but to be efficient for small lists while still
// growable to large lists. Doing this because using a vector of 
// vector<int> for table3 ended up taking up too much memory.
//
// $Id: KeyTable3Element.h,v 1.1 2011-03-03 14:10:58 simeon Exp $

#include <iostream>

class KeyTable3Element {
  int max;
  int last;
  int* x;

public:
  KeyTable3Element(void);
  KeyTable3Element(int n);
  ~KeyTable3Element(void);
  void push_back(int i);
  int back(void);
  int size(void);
  int capacity(void);
  int* begin(void);
  int* end(void);
  int operator[](int i);
  // my addition
  int size_in_bytes(void);
  friend std::ostream& operator<<(std::ostream& out, KeyTable3Element& k);

  // Here is a custom iterator which I've based on the example at
  // http://www.oreillynet.com/pub/a/network/2005/11/21/what-is-iterator-in-c-plus-plus-part2.html?page=5
  // This provides mimimal functionality to mimic the STD vector iterator
  //
  class iterator : public std::iterator<std::forward_iterator_tag, KeyTable3Element> {
    public:
      iterator(int* i) { ptr=i; }
      ~iterator() {}

      // The assignment and relational operators are straightforward
      iterator& operator=(const iterator& other)
      {
	ptr = other.ptr;
	return(*this);
      }

      iterator& operator=(int* i)
      {
	ptr = i;
	return(*this);
      }

      bool operator==(const iterator& other)
      {
	return(ptr == other.ptr);
      }

      bool operator!=(const iterator* other)
      {
	return(ptr == other->ptr);
      }

      bool operator!=(int* i)
      {
	return(ptr != i);
      }

      bool operator!=(const iterator& other)
      {
	return(ptr != other.ptr);
      }

      iterator& operator++()
      {
	this->ptr++;
	return(*this);
      }

      iterator& operator++(int)
      {
	this->ptr++;
	return(*this);
      }

      int operator*()
      {
        return(*ptr);
      }

    private:
      int* ptr;
  };

};
