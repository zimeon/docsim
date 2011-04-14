// KeyTable object
// Simeon Warner - 2005-08-03..
//
// Represents a set of short keys (last KEY_BITS of the full kgram key)
// and the list of documents that they occur in (zero or more). Designed
// to use limited memory to allow in-memory lookup to find overlapping
// documents.
//
// Uses three tables as a hierarchy of storage:
//
// table1 [ 0 ... 2^bits-1 ], one integer for each short key
// values 0 ... max_docid -> document id where one document has key
//        -ve             -> pointer into table2
//        EMPTY           -> element not used (use value INT_MAX)
//
// table2 [ 0 ... TABLE2_SIZE-1 ], two integers for each entry
// entries are indexed by -(value in table1)*2 with values of
//        [doc_id1, doc_id2] -> two document with key
//        [-ve, 0 ]          -> pointer into table3 for >2 docs with key
//
// table3 is a vector of vectors of int with values
//        [doc_id1, doc_id2, doc_id3 [[,doc_id4..]] ] at least 3 doc ids
//
// The values in table1 are signed int (32bit) with -ve values pointing
// to entries in table2. This imposes a limitiation on the size of table2
// as having at most 2^31 entries (2,147,483,648).
//
// (It also imposes a limit on the maximum document id of 2^31-1 (2,147,483,647)
// but the whole system will break down long before then. Current code has
// a limit of 1,000,000 built in with strict checks)

#include "definitions.h"
#include "options.h"
#include "kgrams.h"
#include "KeyMap.h"
#include "KeyTable.h"
#include "DocPair.h"
#include "pstats.h"
#include <limits.h>        // for INT_MAX
#include <math.h>          // for pow()
#include <string.h>        // for strlen()
#include <sstream>         // for use in writeMultiFile


// Constructor. In usual use dummy is set to false (not specified) and memory
// for the keyTable is allocated. If dummy is set true then a dummy KeyTable
// object is created with values of KEY_BITS, KEY_DIGITS, KEY_FMT set correctly
// for a KeyTable of the specified number of bit, but no memory for the table
// is allocated.
//
// May be call without dummy, bitmask, bitmaskMatch params
//
KeyTable::KeyTable(int bits, bool dummy, int selectBits, int selectBitsMatch)
{
  int max_bits=28; // 2^28 entries of int = 1GB table1
  string system_size="unknown (assuming 32-bit OS limits)"; 
  if (sizeof(int*) == 4) {
    // for 32-bit system, 2G total data memory limit (on linux at least)
    // must limit table1 to 28bit = 1GB to allow space for other data
    system_size="32-bit OS";
  } else if (sizeof(int*) == 8) {
    // system is 64-bit, set 32-bit limit (~16GB table1)
    // at some stage could run into 2^31 limit on entries in table2 (see notes in header)
    max_bits=32;
    system_size="64-bit OS";
  }
  if (bits<4 || bits>max_bits) {
    cerr << "KeyTable::KeyTable: bit parameter out of range: 4<=bits<=" << max_bits << " (" << system_size << ")" << endl;
    exit(1);
  }
  //
  KEY_BITS=bits;
  KEY_DIGITS=8; // always use 8 for now, could be (bits+3)/4;
  KEY_FMT=(char*)"%08x";
  MAX_INDEX=(int)pow((float)2,bits)-1;       //FIXME: where is an all-int function?
  cerr << "KeyTable::KeyTable: creating table with " << KEY_BITS << " bits, MAX_INDEX = "
       << showbase << hex << MAX_INDEX << dec << " (" << MAX_INDEX << ")" << endl;

  // 
  SELECT_MASK=0;
  SELECT_MATCH=0;
  if (selectBits) {
    if (selectBits<=selectBitsMatch) {
      cerr << "KeyTable::KeyTable: Can't have selectBits(" << selectBits << ") <= selectBitsMatch (" << selectBitsMatch << ")" << endl; 
      exit(2);
    }
    SELECT_MASK=(int)pow((float)2,selectBits)-1-MAX_INDEX; //FIXME: where is an all-int function?
    SELECT_MATCH=selectBitsMatch << bits;
    cerr << "KeyTable::KeyTable: creating table SELECT_MASK=" << hex << SELECT_MASK << " and SELECT_MATCH=" << SELECT_MATCH << dec << endl;
  }
  EMPTY=INT_MAX;
  //
  if (dummy) {
    // Don't actually assign any storage in the dummy table
    TABLE1_SIZE=0;
    TABLE2_SIZE=0;
    table2_size=0;
  } else {
    //
    TABLE1_SIZE=MAX_INDEX+1;
    //
    // Now allocate table of this size... 
    table1=new int [TABLE1_SIZE];
    if (table1==(int*)NULL) {
      cerr << "Error - failed to allocate KeyTable::table1 with size " << TABLE1_SIZE << endl;
      exit(2);
    }
    // Initialize table1 to 0 (no key, no pointer)
    for (int j=0; j<TABLE1_SIZE; j++) {
      table1[j]=EMPTY;
    }
    TABLE2_SIZE=TABLE1_SIZE/4;
    //
    // Now allocate table of this size... 
    table2=new int[TABLE2_SIZE*2];
    if (table2==(int*)NULL) {
      cerr << "Error - failed to allocate KeyTable::table2 with size " << TABLE2_SIZE << endl;
      exit(2);
    }
    // Initialize table2 to 0 (no key, no pointer)
    for (int j=0; j<TABLE2_SIZE; j++) {
      table2[j*2]=0;
      table2[j*2+1]=0;
    }
    table2_size=0;
  }
  maxDocid=-1;
  //
  numDocidsInRead=-1;

  // Set to no-prune
  pruneAbove=0;
}


// Just clean up memory
//
KeyTable::~KeyTable(void)
{
  delete table1;
  delete table2;
}


// Allow extension of table2 if we overrun
//
void KeyTable::growTable2(void)
{
  int old_table2_max=TABLE2_SIZE;
  int* old_table2=table2;
  //
  if (TABLE2_SIZE>TABLE1_SIZE) {
    cerr << "Error - doesn't make sense to grow KeyTable::table2 when size " << TABLE2_SIZE << " already >= table1 size " << TABLE1_SIZE << endl;
    int num_non_ptr=0;
    for (int j=0; j<TABLE1_SIZE; j++) {
      // All entries in table1
      if (table1[j]>=0) {
        if (num_non_ptr==0) {
          cerr << "   FIRST non pointer entry is # " << j << " with value " << table1[j] << endl;
        }
        num_non_ptr++;
      }
    }
    cerr << "   GOT " << num_non_ptr << " non pointer entries in table1 (should be 0)" << endl;
    exit(2);
  } else if ((TABLE2_SIZE*2)>TABLE1_SIZE) {
    // grow to match table1 size, this is the maximum we can use
    TABLE2_SIZE=TABLE1_SIZE+1;
  } else {
    // double
    TABLE2_SIZE=TABLE2_SIZE*2;
  }
  table2=new int[TABLE2_SIZE*2];
  if (table2==(int*)NULL) {
    cerr << "Error - failed to re-allocate KeyTable::table2 with size " << TABLE2_SIZE << endl;
    exit(2);
  }
  // Copy first part of table
  for (int j=0; j<old_table2_max; j++) {
    table2[j*2]=old_table2[j*2];
    table2[j*2+1]=old_table2[j*2+1];
  }
  // Initialize rest of table2 to 0 (no key, no pointer)
  for (int j=old_table2_max; j<TABLE2_SIZE; j++) {
    table2[j*2]=0;
    table2[j*2+1]=0;
  }
  // Finally, delete old table and return
  delete[] old_table2;
}

// Allow code to drop table1 so that we can save space for code the
// doesn't need unique keys to be kept.
//
void KeyTable::dropTable1(void)
{
  delete[] table1;
  TABLE1_SIZE=0;
}

// Since we know that all keys for a specific docid will be added in
// a chunk, we can avoid adding duplicates of a docid by checking the
// last docid associated with the given short key. Return with no 
// action if a dupe is detected.
//
// We also record the highest docid for later use
//
void KeyTable::addKey(kgramkey& key, int docid)
{
  if (SELECT_MASK && (int)(key&SELECT_MASK)!=SELECT_MATCH) {
    return;
  }
  // Chop off high bits with bitwise AND
  int i=(int)(key&MAX_INDEX);
  addKey(i,docid);
}

void KeyTable::addKey(int i, int docid)
{
#ifdef STRICT_CHECKS
  if (i<0 or i>MAX_INDEX) {
    cerr << "KeyTable::addKey: Error - i=" << i << " which is not allowed (must be 0<=i<=" << MAX_INDEX << ")" << endl;
    exit(2);
  }
  if (docid<1 || docid>1000000) {
    cerr << "KeyTable::addKey: Error - bad value for docid=" << docid << endl;
    exit(2);
  }
#endif
  if (docid>maxDocid) maxDocid=docid;
  if (table1[i]==EMPTY) {
    // no entry for this short key, simply add 
    table1[i]=docid;
  } else if (table1[i]==docid) {
    // already in table1, do nothing
  } else {
    // either make new entry in table2 or add element
    table1[i]=addKeyTable2(i,docid);
  }
}


// Adds docid to entry in table2, returns offset into table2
//
// Document docid may already be entered for short key i in table2 (or table3)
// so we have to check for this and skip if found.
//
int KeyTable::addKeyTable2(int i, int docid)
{
#ifdef STRICT_CHECKS
  if (table1[i]==EMPTY) {
    cerr << "KeyTable::addKeyTable2: Error - should not be called with table1[i]==EMPTY, i=" << i << endl;
    exit(2);
  }
#endif
  int i2;
  if (table1[i]>0) {
    // Must create new entry in table2, we already know docid isn't dupe, check 
    // first that we don't need to extend table2
    if (table2_size>=TABLE2_SIZE) {
      growTable2();
      cerr << "KeyTable::addKeyTable2: Warning - reached max size of table2, " << table2_size << " grown to " << TABLE2_SIZE << endl;
    }
    i2=++table2_size;
    table2[(i2-1)*2]=table1[i];
    table2[(i2-1)*2+1]=docid;
  } else {
    // Already have entry in table2 which means that both elements are full
    // must create or add to an entry in table3 if docid isn't dupe
    i2=-table1[i];
    if (table2[(i2-1)*2]<0) {
      // Already have ptr to table3
      table2[(i2-1)*2]=addKeyTable3(i,docid);
    } else if (table2[(i2-1)*2+1]!=docid) {
      table2[(i2-1)*2]=addKeyTable3(i,docid);
    }
  }
  // Return ptr to go in table1
  return(-i2);
}


// Adds docid to entry in table3, returns offset into table3
//
int KeyTable::addKeyTable3(int i, int docid)
{
  int i2=-table1[i];
#ifdef STRICT_CHECKS
  if (table1[i]>=0) {
    cerr << "KeyTable::addKeyTable3: Error - should not be called with table1[i]>=0, i=" << i << endl;
    exit(3);
  }
  if (table2[(i2-1)*2]==0) {
    cerr << "KeyTable::addKeyTable3: Error - should not be called with table2[(i2-1)*2]==0, i2=" << i2 << endl;
    exit(3);
  }
#endif
  int i3;
  if (table2[(i2-1)*2]>0) {
    // Must create new entry in table3, we already know docid isn't dupe
    int initialSize=3;
    if (numDocidsInRead>0) {
      initialSize=numDocidsInRead;
    }
    KeyTable3Element entry(initialSize);
    table3.push_back(entry);
    i3=table3.size(); // there has to be a better way to get index of new element+1
    // Now take entries from table2 and put as first and second in table3[i3-1]
    table3[i3-1].push_back(table2[(i2-1)*2]);
    table3[i3-1].push_back(table2[(i2-1)*2+1]);
  } else {
    // Already have entry in table3, just check against dupe
    i3=-table2[(i2-1)*2];
    if (table3[i3-1].back()==docid) return(-i3);
  }
  // Now simply add to table3 
  table3[i3-1].push_back(docid);
  //cout << "table3[" << i3 << "] added " << docid << " size=" << table3[i3-1].size() << endl;
  //
  // Return ptr to go in table2
  return(-i3);
}


void KeyTable::getDocids(intv& docids, int i)
{
#ifdef STRICT_CHECKS
  if (i<0 || i>=TABLE1_SIZE) {
    cerr << "KeyTable::getDocids: Out of bounds error, attempt to access KeyTable index " << i << " where TABLE1_SIZE=" << TABLE1_SIZE << endl;
    exit(2);
  }
#endif
  if (table1[i]==EMPTY) {
    // no entry for this short key, do nothing
  } else if (table1[i]>0) {
    // one docid in table1
    docids.push_back(table1[i]);
  } else {
    // Something in table2 or table3
    int i2=(-table1[i]-1)*2; //index in table2
    if (table2[i2]>0) {
      // Take the two elements from table2
      docids.push_back(table2[i2]);
      docids.push_back(table2[i2+1]);
    } else if (table2[i2]<0) {
      // copy all of entries from table3
      KeyTable3Element* t3=&table3[-table2[i2]-1];
      for (KeyTable3Element::iterator t3i=t3->begin(); t3i!=t3->end(); t3i++) {
        docids.push_back(*t3i);
      }
    } else {
      cerr << "KeyTable::getDocids: bad table2[" << i2 << "] entry of 0" << endl;
    }
  }
}


// Extract a list of ids that have at least n keys shared with other documents
//
void KeyTable::getOverlapIds(intv& docids, int n)
{
  cout << "KeyTable::getOverlapIds(" << n << ")" << endl;
   
  // Allocate an array where we can count the number of times we see
  // each docid
  if (maxDocid<0) {
    cerr << "KeyTable::getOverlapIds: maxDocid not set..." << endl;
    exit(3);
  }
  int* did=new int[maxDocid+1];
  if (did==(int*)NULL) {
    cerr << "KeyTable::getOverlapIds: Error - failes to allocate did[" << (maxDocid+1) << "] arrary" << endl;
    exit(4);
  }
  // Initialize to all zero
  for (int j=0; j<=maxDocid; j++) {
    did[j]=0;
  }
  
  // Now run through complete table2 and table3 incrementing did[docid] for
  // each time docid appears. We can simply ignore table1 as this has only
  // uniquely occuring keys
  for (int j=0; j<table2_size; j++) {
    if (table2[j*2]>0) {
      did[table2[j*2]]++;
      did[table2[j*2+1]]++;
    } else {
      KeyTable3Element *t3=&table3[-table2[j*2]-1];
      for (KeyTable3Element::iterator t3i=t3->begin(); t3i!=t3->end(); t3i++) {
        did[*t3i]++;
      } 
    }
  }

  // Got through an add docids for which did[docid]>n to docids
  for (int j=1; j<=maxDocid; j++) {
    if (did[j]>n) docids.push_back(j);
  }
   
  cout << "KeyTable::getOverlapIds: looked at " << maxDocid << " ids and returned " 
       << docids.size() << " ids which have at least " << n << " overlaps" << endl;
  delete did;
} 


// Find candidate similar documents based on which pairs of documents
// ids in docids share more than n keys. 
// 
// Note, we could actually undercount matching keys here if two kgrams
// that match between the documents happen to give the same short key and
// thus are counted only once. [Simeon/2005-08-04]
//
void KeyTable::getOverlapDocs(DocPairVector& docpairs, intv& docids, int n)
{
  // Easiest to work in docid space here rather than in the indexes of
  // the docids array. This is wasteful
  int* overlap=new int[maxDocid+1];
  if (overlap==(int*)NULL) {
    cerr << "KeyTable::getOverlapDocs: Error - failes to allocate overlap[" << (maxDocid+1) << "] arrary" << endl;
    exit(4);
  }
  
  cout << "KeyTable::getOverlapDocs(" << n << ")" << endl;
  for (int j=1; j<((int)docids.size()-1); j++) {
    // Clear overlap array (only need to worry about entries > j)
    for (int k=(j+1); k<=maxDocid; k++) {
      overlap[k]=0;
    }
    // Look for overlap of docid[j] with all docids>docid[j] (avoid double counting)
    // Note that we know that in both table2 and table3 arrays, they ids are in 
    // ascending order. Thus we need only add ids that occur after that one matching
    // j
    for (int k=0; k<table2_size; k++) {
      if (table2[k*2]>0) {
        if (table2[k*2]==j) {
          overlap[table2[k*2+1]]++;
        }
      } else {
	KeyTable3Element *t3=&table3[-table2[k*2]-1];
	bool gotMatch=false;
        for (KeyTable3Element::iterator t3i=t3->begin(); t3i!=t3->end(); t3i++) {
	  if (!gotMatch) {
            if (*t3i==j) gotMatch=true;
	  } else {
	    overlap[*t3i]++;
	  }
	}
      } 
    }
    //
    // Run though overlap array an pick out documents which have overlap=>n
    // with document j
    for (int k=(j+1); k<=maxDocid; k++) {
      if (overlap[k]>=n) { 
        DocPair d(j,k,overlap[k]);
        docpairs.push_back(d);
      }
    }
    if (j%100==0) cout << "KeyTable::getOverlapDocs[" << j << "] Got " << docpairs.size() << " pairs so far" << endl;
  }
  cout << "Found " << docpairs.size() << " document pairs sharing >= " << n << " keys" << endl;
  delete overlap;
}


// Given the current KeyTable (representing the keys in the corpus), find the
// overlap with keys in the keymap kms and put all of these in kmd.
//
void KeyTable::getOverlapKeys(keymap& kms, keymap& kmd)
{
  intv indexes;
  keysToIndexes(kms,indexes);
  getOverlapKeys(indexes,kmd);
}


void KeyTable::getOverlapKeys(intv& indexes, keymap& kmd)
{
  intv docids;
  for (unsigned int j=0; j<indexes.size(); j++) {
    docids.clear();
    getDocids(docids,indexes[j]);
    if (docids.size()>0) {
      // There is overlap and we have a list of docids to insert
      KgramInfo* kip=new KgramInfo(docids);
#ifdef STRICT_CHECKS
      // There should not be any occasion where there is already a value
      // in the KeyMap with the same key (index). If this occurs then
      // the kmd.insert(..) below will do nothing and will leave the newly
      // create KgramInfo object kip as a memory leak. In this test we 
      // check to see whether there is an existing value and replace doing 
      // cleanup if there is.
      // FIXME -- why do we get any repeats??? 
      keymap::iterator kmdi=kmd.find((kgramkey)indexes[j]);
      if (kmdi!=kmd.end()) {
        cerr << "KeyTable: Doing replace of replace of index " << indexes[j] << endl;
        KgramInfo* old_kip=kmdi->second;
        cerr << "KeyTable: < " << *old_kip << endl << "KeyTable: > " << *kip << endl;
        delete kmdi->second; // else this is memory leak
        kmd.erase(kmdi);
      }
#endif
      kmd.insert(keymap::value_type((kgramkey)indexes[j],kip));
    }
  }
}

 
void KeyTable::writeStats(ostream& out) {
  int numTable1=0;
  int ptrTable1=0;
  int numTable2=0;
  int numTable3=0;
  int totTable3=0;
  int minTable3=9999999;
  int maxTable3=0;

  float t1_mem= sizeof(*table1)*TABLE1_SIZE / (1024.0*1024.0);    // 1 int per entry
  float t2_mem= sizeof(*table2)*TABLE2_SIZE*2 / (1024.0*1024.0);  // 2 ints per entry
  float t3_mem= sizeof(int)*table3.capacity() / (1024.0*1024.0);  // 1 ptr per entry (+ vectors later)

  // Count up everything for table1 if it exists
  for (int j=0; j<TABLE1_SIZE; j++) {
    if (table1[j]==EMPTY) {
      // nothing, unused
    } else if (table1[j]>0) {
      numTable1++;
    } else {
      ptrTable1++;
    }
  }

  // ...and table2 and table3 (which will still be counted correctly
  // if there is no table1)
  for (int j=0; j<table2_size; j++) {
    if (table2[j*2]>0) {
      numTable2++;
    } else {
      numTable3++;
      int s=table3[-table2[j*2]-1].size();
      totTable3+=s;
      t3_mem+=table3[-table2[j*2]-1].size_in_bytes();
      if (s>maxTable3) maxTable3=s;
      if (s<minTable3) minTable3=s;
    }
  }

  // Calculate % of keys in each table
  float keys_pct = (numTable1 + numTable2 + table3.size() ) / 100.0;
  cout.precision(3);

  // Go through each table and write out details if it exists
  if (TABLE1_SIZE>0) {
    out << "KeyTable::writeStats(table1): numTable1=" << numTable1
        << " (" << (numTable1/keys_pct) << "%)"
        << " ptrTable1=" << ptrTable1 
        << " TABLE1_SIZE=" << TABLE1_SIZE << endl;
    if (ptrTable1!=table2_size) {
      out << "KeyTable::writeStats(table1): WARNING: ptrTable1!=table2_size\n";
    }
  } else {
    out << "KeyTable::writeStats(table1): none" << endl;
  }
  if (table2_size>0) {
    out << "KeyTable::writeStats(table2): numTable2=" << numTable2
        << " (" << (numTable2/keys_pct) << "%)"
        << " ptrTable2=" << numTable3 
        << " table2_size=" << table2_size << " TABLE2_SIZE=" << TABLE2_SIZE << endl;
  } 
  if (numTable3>0) {
    out << "KeyTable::writeStats(table3): numTable3=" << table3.size() 
        << " (" << (table3.size()/keys_pct) << "%)"
        << " min=" << minTable3 
        << " max=" << maxTable3 << " ave=" << ((float)totTable3/(float)numTable3) << endl;
  } 

  // Memory usage from C++ perspective
  t3_mem=int ( t3_mem / (1024.0*1024.0) + 0.5);
  cout.precision(4);
  out << "KeyTable::writeStats(memory): data=" << (t1_mem+t2_mem+t3_mem)
      << "MB: table1=" << t1_mem 
      << "MB, table2=" << t2_mem 
      << "MB, table3=" << t3_mem
      << "MB" << endl;
  
  // Memory usage from system persective
  out << "KeyTable::writeStats(system): " << get_pstats_string() << endl; 

}


// Extract keys from input keymap and add to list of KeyTable indexes
// in indexes
//
void KeyTable::keysToIndexes(keymap& km, intv& indexes)
{
  for (keymap::iterator kmit=km.begin(); kmit!=km.end(); kmit++) {
    int i=(int)((kmit->first)&MAX_INDEX);
    indexes.push_back(i);
  }
}



//=======================================================================================
// Write KeyTable routines
//

// Returns number of chars in x when printed as a decimal
// Max for unsigned int is 10 chars
int numChars(unsigned int x) {
  if (x<10) return(1);
  if (x<100) return(2);
  if (x<1000) return(3);
  if (x<10000) return(4);
  if (x<100000) return(5);
  if (x<1000000) return(6);
  if (x<10000000) return(7);
  if (x<100000000) return(8);
  if (x<1000000000) return(9);
  return(10);
}

// Write complete KeyTable (tables 1, 2 and 3) to out
//
int KeyTable::writeTables123(ostream& out, int* positionPtr, long int bytes) {
  char buf[10];
  long int bytesWritten=0;
  // Sanity check, barf if no table1
  if (TABLE1_SIZE<=0) {
    cerr << "KeyTable::writeTables123: Attempt to write KeyTable with no/empty table1, nothing written\n";
    return(bytesWritten);
  }
  // Now see if we want just a chunk
  int startPosition=0;
  bool chunk=false;
  if (positionPtr!=(int*)NULL && bytes>0) {
    startPosition=*positionPtr;
    chunk=true;
  }
  // Normal, table1,2,3...
  int j;
  for (j=startPosition; j<TABLE1_SIZE; j++) {
    if (table1[j]!=EMPTY) {
      sprintf(buf,KEY_FMT,(j|SELECT_MATCH));
      if (table1[j]>=0) {
        out << buf << " " << table1[j] << endl;
        bytesWritten+=10+numChars(table1[j]);
      } else {  // table1[j]<0 => entry in table2
        int i2=-table1[j];
        if (table2[(i2-1)*2]>=0) {
          out << buf << " " << table2[(i2-1)*2] << " " << table2[(i2-1)*2+1] << endl;
          bytesWritten+=10+numChars(table2[(i2-1)*2])+numChars(table2[(i2-1)*2+1]);
        } else {  // table2[(i2-1)*2]<0 => entry in table3
          out << buf;
          bytesWritten+=9; //include endl
          KeyTable3Element *t3=&table3[-table2[(i2-1)*2]-1];
          for (KeyTable3Element::iterator t3i=t3->begin(); t3i!=t3->end(); t3i++) {
            out << " " << *t3i;
            bytesWritten+=1+numChars(*t3i);
          } 
          out << endl;
        }
      }
    }
    if (chunk && bytesWritten>=bytes) break;
  }
  // Sort out position if we were writing a chunk
  if (chunk) {
    j++;
    while (table1[j]==EMPTY && j<TABLE1_SIZE) {
      j++; // Skip any following empty positions
    }
    if (j>=TABLE1_SIZE) { // complete
      *positionPtr=-1;
    } else {
      *positionPtr=j;
    }
  }
  return(bytesWritten);
}


// Write just tables 2 and 3.
//
// Without table1 we don't know which short keys docs share, 
// just that they share them
//
int KeyTable::writeTables23(ostream& out, int* positionPtr, long int bytes) {
  char buf[10];
  long int bytesWritten=0;
  // Now see if we want just a chunk
  int startPosition=0;
  bool chunk=false;
  if (positionPtr!=(int*)NULL && bytes>0) {
    startPosition=*positionPtr;
    chunk=true;
  }
  int j;
  for (j=startPosition; j<table2_size; j++) {
    sprintf(buf,"XX%06x",j);
    if (table2[j*2]>0) {
      out << buf << " "  << table2[j*2] << " " << table2[j*2+1] << endl;
      bytesWritten+=10+numChars(table2[j*2])+numChars(table2[j*2+1]);
    } else {
      out << buf;
      bytesWritten+=9; //include endl
      KeyTable3Element* t3=&table3[-table2[j*2]-1];
      for (KeyTable3Element::iterator t3i=t3->begin(); t3i!=t3->end(); t3i++) {
         out << " " << *t3i;
         bytesWritten+=1+numChars(*t3i);
      } 
      out << endl;
    }
    if (chunk && bytesWritten>=bytes) break;
  }
  // Sort out position if we were writing a chunk
  if (chunk) {
    if ((++j)>=table2_size) { // complete
      *positionPtr=-1;
    } else {
      *positionPtr=j;
    }
  }
  return(bytesWritten);
}


// Write table to multiple files of up to maxFileSize bytes each
//
int KeyTable::writeMultiFile(string& baseName, bool allTables, long int maxFileSize) {
  int numFiles=0;
  int position=0;
  long int bytesWritten=0;
  do {
    numFiles++;
    ostringstream fileName;
    fileName << baseName << "_" << numFiles << ".keytable";
    ofstream ktout;
    ktout.open(fileName.str().c_str(),ios_base::out);
    if (!ktout.good()) {
      cerr << "KeyTable::writeMultiFile: Error - can't write to " << fileName.str() << endl;
      exit(2);
    }
    if (allTables) {
      bytesWritten+=writeTables123(ktout,&position,maxFileSize);
    } else {
      bytesWritten+=writeTables23(ktout,&position,maxFileSize);
    }
  } while (position>0);
  cout << "KeyTable::writeMultiFile: wrote " << bytesWritten << " in " << numFiles << " files." << endl;
  return(numFiles);
}


void KeyTable::writeIndexes(ostream& out, intv& indexes)
{
  char buf[10];
  for (unsigned int j=0; j<indexes.size(); j++) {
    sprintf(buf,KEY_FMT,indexes[j]);
    out << buf << endl;
  }
}


//=======================================================================================
// Read KeyTable routines
//

// Simple setter method fro pruneAbove Control
//
void KeyTable::setPruneAbove(int p)
{
  if (p<0 || p>1000000) {
    cerr << "KeyTable::pruneAbove: non-sensical pruneAbove value '" << p << "', aborting!" << endl;
    exit(2);
  }
  pruneAbove=p;
}


// Set to not prune
//
void KeyTable::noPrune(void)
{
  setPruneAbove(0);
}


bool KeyTable::readDocidList(istream& in, intv &docids)
{
  int ch;
  char buf[12];
  while ((ch=in.get())==' ' || (ch>='0' && ch<='9')) {
    in.putback(ch);    
    while ((ch=in.get())==' ') { /*nuttin*/ }
    in.putback(ch);
    int j;
    for (j=0; (((ch=in.get())>='0' && ch<='9') && j<KEY_DIGITS); j++) { 
      buf[j]=(char)ch;
    }
    buf[j]='\0';
    in.putback(ch);
    if (j<KEY_DIGITS) {
      int docid=atoi(buf);
      docids.push_back(docid);
    }
  }
  if (ch!='\n') {
    // We got to some illegal character instead of to the end of line, barf
    cerr << "KeyTable::readDocidList: Error - bad char '" << (char)ch << "' (" << ch << ") in docid list" << endl;
    return(false);
  }
  return(true);
}

// Parse index to table1 from string (KEY_DIGITS byte hex)
//
int KeyTable::stringToIndex(char* keystr)
{
  if ((int)strlen(keystr)!=KEY_DIGITS) {
    cerr << "KeyTable::readIndex: bad index length, " << strlen(keystr) << " not " << KEY_DIGITS << " chars" << endl;
    exit(1);
  }
  int key=0;
  for (int j=0; j<KEY_DIGITS; j++) {
    int d;
    if (keystr[j]>='0' and keystr[j]<='9') {
      d=(int)keystr[j]-(int)'0';
    } else if (keystr[j]>='a' and keystr[j]<='f') {
      d=(int)keystr[j]-(int)'a'+10;
    } else {
      cerr << "KeyTable::readIndex: bad character '" << keystr[j] << "' in key" << endl;
      exit(1);
    }
    key=(key<<4)+d;
  }
  return(key);
}


// Reads data from a KeyTable format file and fills either a KeyTable (if km ptr
// is NULL) or a keymap (is km ptr is not NULL). A dummy KeyTable object of the
// correct size to correspond with the file being read is adequate is a keymap
// is being created
//
// Returns the number of keys added to the KeyTable/keymap
//
int KeyTable::readTables123(istream& in, intv* filterKeys, keymap* km)
{
  // Sanity check
  if (!in || in.eof()) {
    cerr << "KeyTable::readTables123: istream in is no good, aborting!" << endl;
    exit(2);
  }
  //
  bool* filterLookup=(bool*)NULL;
  int maxKey=0;
  if (filterKeys!=(intv*)NULL) {
    // Find largest key value in filterKeys
    for (unsigned int j=0; j<filterKeys->size(); j++) {
      if ((*filterKeys)[j]>maxKey) maxKey=(*filterKeys)[j];
    }
    // Create filterLookup array up to and including filterKeys
    filterLookup=new bool[maxKey+1];
    if (filterLookup==(bool*)NULL) {
      cerr << "KeyTable::readTables123: Error - failed to allocate bool filterLookup[" << (maxKey+1) << "]" << endl;
      exit(2);
    }
    for (int j=0; j<=maxKey; j++) {
      filterLookup[j]=false;
    }
    for (unsigned int j=0; j<filterKeys->size(); j++) {
      filterLookup[(*filterKeys)[j]]=true;
    }
    cout << "KeyTable::readTables123: created bool[" << (maxKey+1) << "] filterLookup array" << endl;
  }
  int numKeys=0;
  int line=0;
  char* buf = new char[18];
  int key;
  intv docids;
  while (in && !in.eof()) {
    line++;
    //===== Get kgramkey =====
    int j;
    for (j=0; in && j<KEY_DIGITS; j++) {
      buf[j]=in.get();
    }
    buf[j]='\0';
    if (j==KEY_DIGITS) {
      if (VERY_VERBOSE) cout << "KeyTable::readTables123[" << line << "] about to parse index key " << buf << endl;
      key=stringToIndex(buf);
    } else {
      cerr << "KeyTable::readTables123[" << line << "] bad line, can't get index key, got '" << buf << "'" << endl;
      exit(2);
    }
    //===== Get docids =====
    docids.clear();
    if (!readDocidList(in,docids)) {
      cerr << "KeyTable::readTables123[" << line << "] bad line, error reading docid list" << endl;
      exit(2);
    }
    if (VERY_VERBOSE || (VERBOSE && (line%1000000==0))) { 
      cout << "KeyTable::readTables123[" << line << "] read " << kgramkeyToString(key) << " with " << docids.size() << " ids, ";
      if (km==(keymap*)NULL) {
        cout << "KeyTable stats:" << endl;
        writeStats(cout);
      } else {
        cout << "keymap has " << km->size() << " entries" << endl;
      }
    }
    //
    // Check key against supplied list if filterKeys, ignore this entry if 
    // list is given by there is no match
    if ( (pruneAbove==0 || (int)docids.size()<=pruneAbove) &&
         (filterKeys==(intv*)NULL || (key<=maxKey && filterLookup[key])) ) {
      //===== Add key and ids to KeyTable or keymap =====
      if (km==(keymap*)NULL) {
        // KeyTable...
        numDocidsInRead=docids.size()+1+docids.size()/4;
        for (unsigned int j=0; j<docids.size(); j++) {
          addKey(key,docids[j]);
        }
      } else {
        // keymap...
        //cout << "km->insert(" << kgramkeyToString(key) << ",...)" << endl;
        KgramInfo* kip=new KgramInfo(docids);
        km->insert(keymap::value_type((kgramkey)key,kip));
      }
    }
    //
    int ch;
    if (in && (ch=in.get())) { in.putback(ch); } // read ahead to set in false if next to end    
  }
  //
  // Delete large array we created for lookup, and buffer
  delete[] filterLookup;
  delete[] buf;
  // Reset control for how table3 elements are added
  numDocidsInRead=-1;
  //
  return(numKeys);
}


// Format is dummy number (must be in sequence but not necessarily complete) and 
// then document list on each line. Dummy number is 'XX######' where # is a hex digit.
// 
int KeyTable::readTables23(istream& in, intv* filterKeys, keymap* km)
{
  cerr << "KeyTable::readTables23 NOT YET IMPLEMENTED!!" << endl;
  exit(99);
  int numKeys=0;
  return(numKeys);
}


int KeyTable::readMultiFile(string& baseName, intv* filterKeys, keymap* km)
{
  int numFiles=0;
  bool allTables=true;
  int numKeys=0;
  do {
    numFiles++;
    ostringstream fileName;
    fileName << baseName << "_" << numFiles << ".keytable";
    ifstream ktin;
    ktin.open(fileName.str().c_str(),ios_base::in);
    if (ktin.good()) {
      int ch=ktin.get();
      ktin.putback(ch);
      if (numFiles==1)  {
        // Set type from first char of first file
        allTables=(ch!='X');
        cout << "KeyTable::readMultiFile: First char is '" << (char)ch << "', expecting tables";
        if (allTables) { cout << "123" << endl; } else { cout << "23" << endl; }
      } else if (allTables!=(ch!='X')) {
        cout << "KeyTable::readMultiFile: Filetype mistmatch in file " << fileName << ", first char is '" << (char)ch << "'" << endl;
        exit(2);
      }
      if (allTables) {
        numKeys+=readTables123(ktin,filterKeys,km);
      } else {
        numKeys+=readTables23(ktin,filterKeys,km);
      }
    } else if (numFiles>1) {
      // Already read 1 file, assume just read all 
      numFiles--;
      break;
    } else {
      // Bad and not read anything, barf
      cerr << "KeyTable::readMultiFile: Error - can't read from first file " << fileName.str() << endl;
      exit(2);
    }
  } while (true);
  cout << "KeyTable::readMultiFile: read " << numFiles << " files." << endl;
  return(numFiles);
}


//=======================================================================================
// Friends
//=======================================================================================


// Need to cope also with case of no table1, test for TABLE1_SIZE==0 [Simeon/2005-08-05]
//
ostream& operator<<(ostream& out, KeyTable& k) {
  if (k.TABLE1_SIZE>0) {
    // Normal, table1,2,3...
    k.writeTables123(out);
  } else {
    // No table1, just do table2 and table3
    k.writeTables23(out);
  }
  return(out);
}


// Read in data as written by operator<<
//
// maxDocid is recorded
//
// First character is examined to determin whether this is a table123 or table23
// data dump. If it is 'X' then we assume table23.
//
istream& operator>>(istream& in, KeyTable& k) {
  int ch=in.get();
  in.putback(ch);
  if (ch=='X') {
    k.readTables23(in,(intv*)NULL);
  } else {
    k.readTables123(in,(intv*)NULL);
  }
  return(in);
}
