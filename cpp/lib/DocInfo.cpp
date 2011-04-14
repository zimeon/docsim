// DocInfo object encapsulates information relating to one
// document and has associated methods from processing
// single documents.
// Simeon Warner - 2005-07...
// $Id: DocInfo.cpp,v 1.3 2011-03-09 23:55:17 simeon Exp $

#include "definitions.h"
#include "options.h"
#include "files.h"
#include "kgrams.h"
#include "DocInfo.h"
#include "KgramInfo.h"

DocInfo::DocInfo(const string& fn, const docid i)
{
  filename=fn;
  id=i;
}

DocInfo::~DocInfo(void)
{
}


// Open file for this document and then call routine to read and
// add keys to map
//
void DocInfo::addToKeymap(keymap& keys, int maxDupesToCount, bool winnow)
{
  ifstream fin;     //stream associated with the file
  fin.open(filename.c_str(),ios_base::in);
  if (!fin.is_open()) {
    cerr << "Error - failed to read from '" << filename << "'\n";
    exit(2);
  }
  addToKeymap(fin,keys,maxDupesToCount,winnow);
  fin.close();
}


// Read doc from file and process each line adding all winnowed 
// kgram keys to the keymap keys (with the docid) 
//
// Extra code inserted if DOCUMENT_STATS set
//
void DocInfo::addToKeymap(istream& in, keymap& keys, int maxDupesToCount, bool winnow)
{
#ifdef DOCUMENT_STATS
  int linesInDoc=0;
  int linesUsedInDoc=0;
  int wordsInDoc=0;
  int wordsUsedInDoc=0;
  int charsInDoc=0;
  int charsUsedInDoc=0;
  int kgramsInDoc=0;
#endif

  char* buf;
  kgramkey* kgrams;
  while ((buf=readLine(in))!=(char*)NULL) {
    kgrams = getKgrams(buf,winnow);
    if (kgrams!=(kgramkey*)NULL) {
      for (kgramkey* k=kgrams; *k!=0; k++) {
#ifdef DOCUMENT_STATS
        kgramsInDoc++;
#else
        keymap::iterator kit = keys.find(*k);
        if (kit==keys.end()) {
          // create new entry with just current docid
          keys.insert(keymap::value_type(*k,new KgramInfo(id)));
        } else {
          // get pointer to docidhashset and add extra element if size()<maxDupesToCount
          kit->second->addOccurrence(id,maxDupesToCount);
        }
#endif
      }
    }
#ifdef DOCUMENT_STATS
    int charsInSentence=strlen(buf)+1; //+1 to include trailing '\0'
    int wordsInSentence=0;
    for (int j=0;j<charsInSentence;j++) {
      charsInDoc++;
      if (buf[j]=='\0' || buf[j]==' ') wordsInSentence++;
    }
    linesInDoc++;
    wordsInDoc+=wordsInSentence;
    charsInDoc+=charsInSentence;
    if (wordsInSentence>=MINSENL) {
      linesUsedInDoc++;
      wordsUsedInDoc+=wordsInSentence;
      charsUsedInDoc+=charsInSentence;
    }
#endif
  }
#ifdef DOCUMENT_STATS
  // Write out the stats for this document. The first three numbers should be
  // the counts of lines,words,bytes as given by wc. Last number is the number
  // of winnowed kgrams
  cout << "doc[" << filename << "," << id << "]: " 
       << linesInDoc << " " << linesUsedInDoc << " "
       << wordsInDoc << " " << wordsUsedInDoc << " "
       << charsInDoc << " " << charsUsedInDoc << " "
       << kgramsInDoc << endl;
#endif
}



// Read doc from file and process each line adding all winnowed 
// kgram keys to the KeyTable (with the docid) 
//
// Extra code inserted if DOCUMENT_STATS set
//
void DocInfo::addToKeyTable(KeyTable& kt, int maxDupesToCount)
{
  ifstream fin;     //stream associated with the file
  fin.open(filename.c_str(),ios_base::in);
  if (!fin.is_open()) {
    cerr << "Error - failed to read from '" << filename << "'\n";
    exit(2);
  }

#ifdef DOCUMENT_STATS
  int linesInDoc=0;
  int linesUsedInDoc=0;
  int wordsInDoc=0;
  int wordsUsedInDoc=0;
  int charsInDoc=0;
  int charsUsedInDoc=0;
  int kgramsInDoc=0;
#endif

  char* buf;
  kgramkey* kgrams;
  while ((buf=readLine(fin))!=(char*)NULL) {
    kgrams = getKgrams(buf);
    if (kgrams!=(kgramkey*)NULL) {
      for (kgramkey* k=kgrams; *k!=0; k++) {
#ifdef DOCUMENT_STATS
        kgramsInDoc++;
#else
        kt.addKey(*k,id);
#endif
      }
    }
#ifdef DOCUMENT_STATS
    int charsInSentence=strlen(buf)+1; //+1 to include trailing '\0'
    int wordsInSentence=0;
    for (int j=0;j<charsInSentence;j++) {
      charsInDoc++;
      if (buf[j]=='\0' || buf[j]==' ') wordsInSentence++;
    }
    linesInDoc++;
    wordsInDoc+=wordsInSentence;
    charsInDoc+=charsInSentence;
    if (wordsInSentence>=MINSENL) {
      linesUsedInDoc++;
      wordsUsedInDoc+=wordsInSentence;
      charsUsedInDoc+=charsInSentence;
    }
#endif
  }
  fin.close();
#ifdef DOCUMENT_STATS
  // Write out the stats for this document. The first three numbers should be
  // the counts of lines,words,bytes as given by wc. Last number is the number
  // of winnowed kgrams
  cout << "doc[" << filename << "," << id << "]: " 
       << linesInDoc << " " << linesUsedInDoc << " "
       << wordsInDoc << " " << wordsUsedInDoc << " "
       << charsInDoc << " " << charsUsedInDoc << " "
       << kgramsInDoc << endl;
#endif
}


// Look for kgramkey key in this document, returns (char*) string or NULL
// if not found
//
// If bits is specified as a positive value then this is used to mask
// the kgramkeys in the document so that only the last bits bits are
// matched. This allows one to search for truncated kgramkey values. 
//
char* DocInfo::findKgramInDoc(kgramkey key, int bits)
{
  ifstream fin;     //stream associated with the file
  fin.open(filename.c_str(),ios_base::in);
  if (!fin.is_open()) {
    cerr << "Error - failed to read from '" << filename << "'" << endl;
    exit(2);
  }

  // Build so that mask is 0 or a set of bits 1's in the low order bits
  kgramkey mask=0;
  if (bits>(KGRAMKEYDIGITS*4)) {
    cerr << "Error - number of bits to use in findKgramInDoc mask (" << bits << ") is larger than key size" << endl;
    exit(2);
  } else if (bits>0) {
    for (int j=0; j<bits; j++) {
      mask = (mask<<1 | 1);
    }
  }

  char* buf;
  char* match;
  int line=0;
  while ((buf=readLine(fin))!=(char*)NULL) { 
    if (VERY_VERBOSE) cout << "line[" << line++ << "]: " << buf << "\n";
    if (mask>0) {
      match = findKgramWithMask(key,mask,buf);
    } else {
      match = findKgram(key,buf);
    }
    if (match!=(char*)NULL) {
      fin.close();
      return(match);
    }
  }
  fin.close();
  return((char*)NULL);
}


void DocInfo::markupDoc(ostream& out, keyhashset& keys)
{
  ifstream fin;     //stream associated with the file
  fin.open(filename.c_str(),ios_base::in);
  if (!fin.is_open()) {
    cerr << "Error - failed to read from '" << filename << "'\n";
    exit(2);
  }
  
  char* buf;
  int line=0;
  intv spaces;
  intv words;
  kgramkeyv kgramstarts;
  while ((buf=readLine(fin))!=(char*)NULL) {
    if (VERY_VERBOSE) cout << "line[" << line++ << "]: " << buf << "\n";
    int wordsMatching = findWordsInKgrams(spaces,words,kgramstarts,keys,buf);
    if (wordsMatching<0) {
      out << "[" << buf << "]" << endl;
    } else if (wordsMatching==0) {
      out << buf << endl;
    } else {
      for (int j=0; j<(int)words.size(); j++) {
        if (words[j]>0) {
	  for (int k=(spaces[j]+1); k<(spaces[j+1]); k++) {
	    buf[k]=buf[k]+(int)'A'-(int)'a';
	  }
	}
      }
      out << buf << endl;
    }
  }
  fin.close();
}


void DocInfo::markupCompleteDoc(MarkedDoc& mud, keyhashset& keys)
{
  ifstream fin;     //stream associated with the file
  fin.open(filename.c_str(),ios_base::in);
  if (!fin.is_open()) {
    cerr << "Error - failed to read from '" << filename << "'\n";
    exit(2);
  }
  
  char* buf;
  int line=0;
  intv spaces;
  intv words;
  kgramkeyv kgramstarts;
  while ((buf=readLine(fin))!=(char*)NULL) {
    if (VERY_VERBOSE) cout << "line[" << line++ << "]: " << buf << "\n";
    (void)findWordsInKgrams(spaces,words,kgramstarts,keys,buf);
    mud.addLine(spaces,words,kgramstarts,buf);
  }
  fin.close();
}
