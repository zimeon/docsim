// Global definitions for similarity/plagiarism code
//
// Based on code written by and copyright Daria Sorokina, 2005.
// Simeon Warner - 2005-07..
//
// $Id: definitions.h,v 1.4 2011-02-16 23:18:37 simeon Exp $

#ifndef __INC_definitions
#define __INC_definitions 1

#include <iostream>
#include <vector>
#include <string>
#include <ext/hash_set>
#include <ext/hash_map>
#include <limits.h>

using namespace std;
using __gnu_cxx::hash_set;
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_multimap;
#ifdef __LINUX__
  // gcc stl libraries do not support unsigned long long hash functions
  #include "hash_ull.h"
#endif

// Should put in checks to see that these really are the sizes we think
// they are. How to do this at compile time?
typedef unsigned long long U64;
typedef unsigned int U32;
typedef unsigned short U16;
typedef unsigned char U8;
#define U16_MAX USHRT_MAX

// Defintions intimately tied to the key type and size
typedef U64 kgramkey;      // Type for kgram keys
#define KGRAMKEYDIGITS 16  // Number of hex digits in kgram key
#define BASE 27            // Number of different symbols+1 
#define PRIME 682551457733942743LL
//large prime number that will not cause overflow 
//in 64 bits variable when used for fingerprinting
//(i.e. less than 683212743470724132)

// Definitions intimately tied to the document ids
typedef U32 docid;      // Type for document ids

typedef vector<kgramkey> kgramkeyv;

typedef hash_set<kgramkey> keyhashset;
typedef hash_set<docid> docidhashset;

// Utility types based on standard types
typedef vector<int> intv;
typedef vector<string> stringv;

// number of different documents for which we record dupes before we stop counting
//
#define MAX_DUPES_TO_COUNT 100

// number of different documents a kgram must appear in before being "common"
//
// NUM_DUPES_TO_BE_COMMON must be larger than or equal to MAX_DUPES_TO_COUNT 
// or nothing will ever be "common", enforce this at compile time:
//
#define NUM_DUPES_TO_BE_COMMON 10

#define fsep '/'                   // path seperator in file names
#define MAX_FILE_SIZE 2100000000   // maximum file size in bytes (2GB on x86/linux)

// as of 2011-02-16 the maximum psv file size for arXiv is ~4.1MB so a buffer
// size larger than this is required to treat all files whole
//
#define FILE_BUFFER_SIZE 5000000

typedef unsigned long int ptr_to_int;  //used to cast ptr to int to print

#endif /* #ifndef __INC_definitions */
