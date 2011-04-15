// Functions that manipulate kgrams
//
// Based on code written by and copyright Daria Sorokina, 2005.
// Modified and extended by Simeon Warner, 2005-07...
//
// $Id: kgrams.cpp,v 1.6 2011-03-10 01:18:49 simeon Exp $
//
#include "definitions.h"
#include "options.h"
#include "kgrams.h"
#include <string.h>  // for strlen()
#include <ctype.h>

#define INITIAL_MAX_KEYS 800
#define INITIAL_MAX_RESULTS 400

// Returns the number of symbol in the sequence a-z (1-26), 0 otherwise.
//
// We consider only lower case letters, the input file should already be
// prepared in this way.
//
inline int charToInt(char c) { 
  if ((c>='a')&&(c<='z')) return(1+c-'a');
  return(0);
}


// hash-function, from char* to kgramkey (char* must be terminated with \0)
//
// Only latin letters are taken into consideration, the fingerprint can never
// be zero.
//
kgramkey fingerprint(char* startch, char* endch)
{
#ifdef STRICT_CHECKS
  if (startch>=endch) {
    cerr << "fingerprint: STRICT_CHECKS: bad params: startch=" 
         << (unsigned long int)startch << " >=  endch=" << (unsigned long int)endch << "\n";
  }
#endif
  kgramkey res = 0;
  for (char* c=startch; c<=endch; c++) {
    if (int symbol = charToInt(*c)) {
      res = (res*BASE + symbol)%PRIME;
    }
  }
  return(res+1);
}


// Returns a string representation of the hex value of a kgramkey
// 
string kgramkeyToString(U64 hash)
{
  char buf[20];
  U32 hash1 = (U32)(hash>>32);
  U32 hash2 = (U32)(hash&0x0FFFFFFFFLL);
  sprintf(buf,"%08x%08x ",hash1, hash2);
  string ret = buf;
  return ret;
}

// Opposite of kgramkeyToString, extracts kgramkey from input
// string
//
// If the number of chars is not specified (0) then we assume 
// a complete kgramkey and require KGRAMKEYDIGITS hex digits.
// Use this parameter to specify a smaller key.
// 
kgramkey stringToKgramkey(char* keystr, int chars)
{
  if (chars==0) {
    chars=KGRAMKEYDIGITS;
  } else if (chars<8) {
    // Currently use a minimum of 8 hex chars for small keys
    chars=8;
  }
  if ((int)strlen(keystr)!=chars) {
    cerr << "stringToKgramkey: bad kgramkey length, " << strlen(keystr) << ", expected " << chars << " chars" << endl;
    exit(1);
  }
  kgramkey key=0;
  for (int j=0; j<chars; j++) {
    int d;
    if (keystr[j]>='0' and keystr[j]<='9') {
      d=int(keystr[j]-'0');
    } else if (keystr[j]>='a' and keystr[j]<='f') {
      d=int(keystr[j]-'a')+10;
    } else {
      cerr << "stringToKgramkey: bad character '" << keystr[j] << "' in key" << endl;
      exit(1);
    }
    key=(key<<4)+d;
  }
  return(key);
}


// Returns set of kgrams from a sentence following Winnowing algorithm
// Treats words as symbols, not characters.
//
// 2005-07-13 - rewritten to avoid using little hashsets and such 
// internally as they seem to be very inefficient. Now returns
// a pointer to an array of kgrams [Simeon]
//
int MAX_KEYS=INITIAL_MAX_KEYS;
kgramkey* allkeys=new kgramkey[INITIAL_MAX_KEYS];
int MAX_RESULTS=INITIAL_MAX_RESULTS;
kgramkey* results=new kgramkey[INITIAL_MAX_RESULTS];

void growAllkeys() {
  int newMax=MAX_KEYS*2;
  kgramkey* newAllkeys=new kgramkey[newMax];
  for (int j=0; j<MAX_KEYS; j++) newAllkeys[j]=allkeys[j];
  delete[] allkeys;
  if (VERBOSE) {
    cerr << "kgrams::getKgrams: Warning - allkeys array grown from " << MAX_KEYS << " to " << newMax << " keys" << endl;
  }
  allkeys=newAllkeys;
  MAX_KEYS=newMax;
}

void growResults() {
  int newMax=MAX_RESULTS*2;
  kgramkey* newResults=new kgramkey[newMax];
  for (int j=0; j<MAX_RESULTS; j++) newResults[j]=results[j];
  delete[] results;
  if (VERBOSE) {
    cerr << "kgrams::getKgrams: Warning - results array grown from " << MAX_RESULTS << " to " << newMax << " keys" << endl;
  }
  results=newResults;
  MAX_RESULTS=newMax;
}

// Extract kgrams from input sentence. Returns a pointer to static array
// of kgramkey, teminated in a null
//
//
kgramkey* getKgrams(char* sentence, bool winnow)
{
  intv spaces;       //places of word boundaries in the sentence
  int numWords=findSpaces(spaces,sentence);
  //cout << "numWords=" << numWords << ": " << sentence << "\n";
  if (numWords<MINSENL) {
    //cout << "numWords<MINSENL => ignoring sentence\n";
    return((kgramkey*)NULL);
  }

  // build array allkeys with all kgrams
  int numKgrams=0;
  for(int word=0; word<=numWords-WINK; word++) {
    if (numKgrams>=MAX_KEYS) growAllkeys();
    allkeys[numKgrams++]=fingerprint(&sentence[spaces[word]+1],&sentence[spaces[word+WINK]-1]);
  }
  //cout << "Got " << numKgrams << " kgrams from " << numWords << " words\n";

  if (!winnow) {
    // Don't do winnowing, just return all keys
    allkeys[numKgrams]=0;
    return(allkeys);
  }
  
  if (numKgrams<=WINW) {
    // just one kgram
    results[0]=*(findSmallestKgramkey(&allkeys[0],&allkeys[numKgrams-1],(kgramkey*)NULL));
    results[1]=0;
  } else {
    // select one kgram from each window of size (WINW)
    int k;
    int keyNum=0;
    kgramkey* smallestkey;
    kgramkey* lastkey=(kgramkey*)NULL;
    for(k=0; k<=numKgrams-WINW; k++) {
      smallestkey=findSmallestKgramkey(&allkeys[k],&allkeys[k+WINW-1],lastkey);
      if (smallestkey!=lastkey) { 
        // add to list if not same key as last (could still have same value)
        if (keyNum>=MAX_RESULTS) growResults();
        results[keyNum++]=*smallestkey;
        lastkey=smallestkey;
      }
      //cout << "k=" << k << "\n"
    }
    results[keyNum]=0;
  }
  //cout << "results:";
  //for (int k=0; results[k]!=0; k++) { cout << " " << kgramkeyToString(results[k]); }
  //cout << "\n";
  return(results);
}


// Just do a simple linear search for the smallest key in the
// array of kgramkeys from startkey to endkey. Returns the address
// of the smallest kgram key. If startkey==endkey on input then 
// there is only one key, otherwise we have a sequqnce of keys to
// look through.
//
// According to defn 3 in SCHLEIMER+03 we select the kgram lastkey
// to break a tie if that is in the set of options. Otherwise we
// break a tie by selecting the rightmost occurrence as in defn 1
// in SCHLEIMER+03. This reduces the winnowed kgram density to
// 1/WINK in cases where there are many repeated words in a row. For
// example, in hep-ph/9710453, there is the sentence:
//
// gut ln r r r r r r r r r r r r r r r r r r r r r r r r r r r r r 
// r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r
// r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r
// r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r
// r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r 
// r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r 
// r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r r 
// r r r r r r r r r r r r r r r r
//
// lastkey can be set to (kgramkey*)NULL to disable this and always
// select the rightmost of tied keys
//
kgramkey* findSmallestKgramkey(kgramkey* startkey, kgramkey* endkey, kgramkey* lastkey)
{
#ifdef STRICT_CHECKS
  if (startkey>endkey) {
    cerr << "fingerprint: STRICT_CHECKS: bad params: startch="
         << (unsigned long int)startkey << " >=  endch=" << (unsigned long int)endkey << "\n";
  }
#endif
  //cout << "looking for smallest in\n   ";
  //for (kgramkey* k=startkey; k<=endkey; k++) {
  //  cout << " " << kgramkeyToString(*k); 
  //}
  //cout << "\n";
  kgramkey* res=startkey++;
  for (kgramkey* k=startkey; k<=endkey; k++) {
    if ((*k<*res) || ((*k==*res) && (res!=lastkey))) res=k;
  }
  //cout << "got " << kgramkeyToString(*res) << "\n";
  return res;
}


// Find the words that match a given kgramkey.
// Returns 
//   (char*)NULL              if no match
//   (char*)"matching words"  if match
//
char* findKgram(kgramkey& key, char* sentence)
{
  intv spaces;       //places of word boundaries in the sentence
  int numWords=findSpaces(spaces,sentence);
  //cout << "numWords=" << numWords << ": " << sentence << "\n";

  if (numWords<MINSENL) return((char*)NULL); // sentence too small, no match

  // now search through all the kgrams
  for(int word=0; word<=numWords-WINK; word++) {
    if (fingerprint(&sentence[spaces[word]+1],&sentence[spaces[word+WINK]-1])==key) {
      char* match;
      match=new char[spaces[word+WINK]-spaces[word]+1];
      int j;
      for (j=0; j<=(spaces[word+WINK]-spaces[word]-2); j++) {
        match[j]=sentence[spaces[word]+j+1];
      }
      match[j]='\0';
      return(match);
    }
  }
  
  return((char*)NULL); // no match
}


// Find the words that match a given kgramkey where matching if
// checked after applying the bitmask mask to each kgramkey in the
// sentence.
//
// Returns 
//   (char*)NULL              if no match
//   (char*)"matching words"  if match
//
char* findKgramWithMask(kgramkey& key, kgramkey mask , char* sentence)
{
  intv spaces;       //places of word boundaries in the sentence
  int numWords=findSpaces(spaces,sentence);
  //cout << "numWords=" << numWords << ": " << sentence << "\n";

  if (numWords<MINSENL) return((char*)NULL); // sentence too small, no match

  // now search through all the kgrams
  for(int word=0; word<=numWords-WINK; word++) {
    if ((fingerprint(&sentence[spaces[word]+1],&sentence[spaces[word+WINK]-1])&mask)==key) {
      char* match;
      match=new char[spaces[word+WINK]-spaces[word]+1];
      int j;
      for (j=0; j<=(spaces[word+WINK]-spaces[word]-2); j++) {
        match[j]=sentence[spaces[word]+j+1];
      }
      match[j]='\0';
      return(match);
    }
  }
  
  return((char*)NULL); // no match
}


// Find words that are included in the kgram(s) in the set of kgramkeys in
// the set keys. Modifies spaces to be a list of the positions of spaces in
// sentence and words to be an array where each value is the number of kgrams
// the word appeared in. Modifies kgramstarts to contain appropriate krgamkeys
// at the word possitions corresponding to the start of the kgram.
//
// Returns   -1 if sentence was too short
//    else   number of words in kgrams
// 
int findWordsInKgrams(intv& spaces, intv& words, kgramkeyv& keystarts, keyhashset& keys, char* sentence)
{
  int numWords=findSpaces(spaces,sentence);
  words.clear();
  keystarts.clear();
  for (int j=0; j<numWords; j++) {
    words.push_back(0);
    keystarts.push_back(0);
  } 

  if (numWords<MINSENL) return(-1);
  
  // now search through all the kgrams
  for(int word=0; word<=numWords-WINK; word++) {
    kgramkey fp=fingerprint(&sentence[spaces[word]+1],&sentence[spaces[word+WINK]-1]);
    for (keyhashset::iterator kit=keys.begin(); kit!=keys.end(); kit++) {   
      if (fp==(*kit)) {
        // mark all words
	for (int j=word; j<(word+WINK); j++) {
	  words[j]++;
	}
	// record kgramkey in start position
	keystarts[word]=fp;
      }
    }
  }
  
  // add up number of words matching at least one kgram
  int numMatchingWords=0;
  for(int word=0; word<numWords; word++) {
    if (words[word]>0) {
      numMatchingWords++;
    }
  }
  return(numMatchingWords);
}


// Find the spaces in the sentence, returns an integer vector (intv)
// with positions of spaces and the start and end. Modifies the 
// spaces vector to contains the positions.
//
// Returns number of words which is one less than the number of space
// or word boundary positions recorded.
// 
int findSpaces(intv& spaces, char* sentence)
{
  // Record positions of word breaks (spaces), include start and end.
  spaces.clear();
  spaces.push_back(-1);   // beginning is -1
  int len=strlen(sentence);
  for (int j=0; j<len; j++) {
    if (sentence[j]==' ') spaces.push_back(j);
  }
  spaces.push_back(len);  // end is one after last word char
  return(spaces.size()-1);
}



//////////////////////////////////////////////////////////////////////////////////////////
//
// Utility functions


ostream& operator<<(ostream& out, kgramkey& k)
{
  return out << kgramkeyToString(k);
}


ostream& operator<<(ostream& out, kgramkeyv& keys)
{
  for (kgramkeyv::iterator kit=keys.begin(); kit!=keys.end(); kit++) {
    out << kgramkeyToString(*kit) << endl;
  }
  return out;
}


ostream& operator<<(ostream& out, keyhashset& keys)
{
  for (keyhashset::iterator kit=keys.begin(); kit!=keys.end(); kit++) {
    out << kgramkeyToString(*kit) << endl;
  }
  return out;
}


void readKeyhashset(const char* filename, keyhashset& keys) 
{
  ifstream fin;
  fin.open(filename,ios_base::in);
  if (!fin.is_open()) {
    cerr << "Error - failed to read from '" << filename << "'" << endl;
    exit(2);
  }
  //
  //
  int line=0;
  char buf[18];
  while (fin && !fin.eof()) {
    line++;
    // Get kgramkey in buf 
    int j;
    for (j=0; fin && j<16; j++) {
      buf[j]=fin.get();
    }
    buf[j]='\0';
    if (j==16) {
      keys.insert(stringToKgramkey(buf));
    } else {
      // Ignore all whitespace lines, else complain
      bool allWhitespace=true;
      for (int k=0; k<j; k++) {
        if (isalpha(buf[k])) allWhitespace=false;
      }
      if (!allWhitespace) {
        cerr << filename << "[" << line << "] bad line, can't get kgramkey, got '" << buf << "'" << endl;
      }
    }
    // Read and discard rest of line
    while (fin && (fin.get()!='\n')) {
      //nuttin
    }
  }
}
