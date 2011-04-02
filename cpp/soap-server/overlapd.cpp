// SOAPy daemonized version of overlap lookup
//
// Implements two SOAP requests:
//
// <status> -- returns "I_AM_HAPPY" for use as watchdog
//
// <overlap> -- compares document supplied against in memory
// keytable. Returne a set of overlapping documents, one per line,
// where each line is of the form defined by DocPair:
//   docid1 docid2 keys_overlapping
// docid2 will always be 9999999 as a dummy value because the input
// document was supplied directly.
//
// listens for the following signals:
//
// SIGUSR1 -- reload KeyTable and resume
//
// SIGINT (^C), SIGTERM -- quit
//
// Simeon Warner, 2005--
// 2007-02-07 - use gSOAP 2.7.9 for SOAP interface [Simeon]
// 2011-03-02 - tidied, uses logfile, implemented reload on USR1 [Simeon]
//
// $Id: overlapd.cpp,v 1.12 2011-03-10 05:02:18 simeon Exp $
//
#include "options.h"
#include "DocInfo.h"
#include "kgrams.h"
#include "files.h"

#include <unistd.h> // assume GNU getopt
#include <fstream>
#include <sstream>
#include <syslog.h>
#include <time.h>
#include <signal.h>

// For gsoap 
#include "soapH.h"
#include "overlap.nsmap"

#define MY_LOG_FILE "/tmp/overlapd.log"
#define SOAP_PORT 8081

// See fudge in overlap__overlap for return string docs, note that
// this imposed a limit on the return size of about 10k documents
#define DOCS_MAX_CHARS 100000
char docs_buf[DOCS_MAX_CHARS+1];

// Define default file names that may be overriden by command line args
const string myname="overlapd";

// Globals defined in definitions.h and in options.h
KeyTable *global_kt;
ofstream logstream;

void loadKeyTable(KeyTable& kt);

// Handle signals

void sigusr1_handler(int signo)
{
  logstream << myname << ": got SIGUSR1, reloading KeyTable..." << endl;
  loadKeyTable(*global_kt);
  logstream << myname << ": Reread KeyTable" << endl;
}


void sigint_handler(int signo)
{
  logstream << myname << ": got SIGINT, bye, bye." << endl;
  exit(0);
}


void sigterm_handler(int signo)
{
  logstream << myname << ": got SIGTERM, bye, bye." << endl;
  //daemon_close();
  exit(0);
}


// Make in situ modification of \0 terminated char* data str to replace
// all control chars with underscore for debugging output.
//
void strclean(char* str)
{
  for (;*str!='\0';str++) {
    if (*str<' ') *str='_';
  }
}


void loadKeyTable(KeyTable& kt) {
  if (keyTableFile!="") {
    ifstream ktin;
    ktin.open(keyTableFile.c_str(),ios_base::in);
    if (!ktin.good()) {
      cerr << myname << ": Error - failed to open '" << keyTableFile << "' for input, aborting." << endl;
      exit(2);
    }
    ktin >> kt;
    ktin.close();
  } else if (keyTableBase!="") {
    kt.readMultiFile(keyTableBase);
  } else {
    cerr << myname << ": Error - must specify either -t or -T for KeyTable" << endl;
    exit(2);
  }
}

 
int main(int argc, char* argv[])
{
  VERBOSE=0;
  VERY_VERBOSE=0;

  // Read any options
  //
  readOptions(argc, argv, (const char*)"hH?St:T:b:vV", myname, "Run overlap server");

  // Open a log file to append to
  //
  logstream.open(MY_LOG_FILE,ios_base::out|ios_base::app);
  if (!logstream.good()) {
    cerr << myname << ": failed to open log file '" << MY_LOG_FILE << "'" << endl;
    exit(2);
  }
  time_t rawtime;
  time(&rawtime);
  if (VERBOSE) {
    cerr << myname << ": pid " << getpid() << ", writing log to " << MY_LOG_FILE << endl; 
  }
  logstream << endl << "---------------------" << endl;
  logstream << myname << ": pid " << getpid() << ", started at " << ctime(&rawtime) << endl;

  // Redirect cout and cerr to the log  
  cout.rdbuf(logstream.rdbuf());
  cerr.rdbuf(logstream.rdbuf());
  
  // Read data
  //
  KeyTable kt(bitsInKeyTable);
  loadKeyTable(kt);
  global_kt=&kt;
  logstream << myname << ": Read KeyTable" << endl;

  //  // Become a daemon
  
  if (signal(SIGUSR1, sigusr1_handler)==SIG_ERR ||
      signal(SIGINT,  sigint_handler)==SIG_ERR ||
      signal(SIGTERM, sigterm_handler)==SIG_ERR ) {
    logstream << myname << "Failed to set USR1 and TERM handlers" << endl; 
    exit(1);
  }
  //    run_soap_server(myname.c_str())!=0) {
  //  cerr << myname << ": failed run_soap_server(..), errno=" << errno << endl;
  //  exit(1);
  //}

  struct soap soap;
  soap_init(&soap);

  if (1) {
    int m, s; /* master and slave sockets */
    m = soap_bind(&soap, NULL, SOAP_PORT, 100);
    if (m < 0) { 
      soap_print_fault(&soap, stderr);
      exit(-1);
    }
    logstream << myname << ": Socket connection successful: master socket = " << m << endl;
    int count;
    for (count=1; ; count++ ) { 
      s = soap_accept(&soap);
      logstream << myname << ": Socket connection successful: slave socket = " << s 
          << ", count = " << count << endl;
      if (s < 0) {
        soap_print_fault(&soap, stderr);
        exit(-1);
      } 
      soap_serve(&soap);
      soap_end(&soap);
    }
  }
  return 0;
} 


int overlap__overlap(struct soap *soap, char *nat, struct overlap__overlapResponse &response)
{ 
  int bytes;
  bytes=strlen(nat);

  logstream << myname << ": read() read " << bytes << " bytes in input doc" << endl;
  if (VERBOSE) {
    #define SZ 40
    char buf2[SZ+1];
    strncpy(buf2,nat,SZ);
    buf2[SZ]='\0';
    strclean(buf2);
    logstream << myname << ": VERBOSE: data starts '" << buf2 << "'" << endl;
    strncpy(buf2,&nat[bytes-SZ],SZ);
    buf2[SZ]='\0';
    strclean(buf2);
    logstream << myname << ": VERBOSE: data ends   '" << buf2 << "'" << endl;
  }
      
  // "read" document from buffer
  istringstream is(nat);
  DocInfo doc("string",9999999);
  keymap km;
  doc.addToKeymap(is,km);
  logstream << myname << ": Extracted " << km.size() << " keys from input doc" << endl;

  // now find overlap of keys in km with corpus in KeyTable kt
  keymap sharedkeys;
  global_kt->getOverlapKeys(km,sharedkeys);
  logstream << myname << ": Got " << sharedkeys.size() << " overlapping keys" << endl;

  // now find overlapping docs
  DocPairVector dpv;
  getCommonDocs(sharedkeys, dpv, keysForMatch);      
  logstream << myname << ": Found " << dpv.size() << " docs overlapping by >= " << keysForMatch << " keys" << endl;

  // set number of matches in response <matches>
  response.matches = dpv.size();

  // set data about overlaps in response <docs>
  if (dpv.size() == 0) {
    response.docs = (char*)"";
  } else {
    ostringstream osd (ostringstream::out);
    osd << dpv;
    // Copy the response string to a to real and persistent char* data structure. 
    //
    // If we simply do response.docs=(char *)osd.str().c_str()
    // then the response is truncated. See comments at
    // http://stackoverflow.com/questions/1374468/c-stringstream-string-and-char-conversion-confusion
    // which point out that c_str() returns a pointer to a string that is 
    // deleted when the statement ends so we must copy it to something that
    // persists beyond exit of this method.
    //
    strncpy(docs_buf,osd.str().c_str(),DOCS_MAX_CHARS);
    response.docs = docs_buf;
  }

  logstream << myname << ": Returning result, SOAP_OK" << endl << endl;
  return SOAP_OK;
} 


// Status for watchdog
//
int overlap__status(struct soap *soap, char *&status)
{ 
  status = (char*)"I_AM_HAPPY";
  logstream << myname << ": Returning result, SOAP_OK for status call" << endl << endl;
  return SOAP_OK;
} 
