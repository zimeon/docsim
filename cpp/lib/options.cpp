// options.cpp - DocSim command line option handling
//
// Shared code to read command line arguments in all DocSim codes.
// This is all done in a rather verbose an inefficient way but I
// hope it will provide a standard set options/facilities/parameters
// that will make the suite of DocSim programs easier to use.
//
// Simeon Warner - 2005-08-29...
//
// $Id: options.cpp,v 1.14 2011-04-02 01:56:45 simeon Exp $
//
#include "definitions.h"
#include "options.h"
#include <stdlib.h>
#include <unistd.h> // assume GNU getopt
#include <string.h> // for strncpy()
#include <sstream>

#define DEFAULT_BASE_DIR "/tmp"
#define DEFAULT_DATA_DIR "."
#define DEFAULT_KEYS_FOR_MATCH 10
#define DEFAULT_CANDIDATES_FILE "candidates.dpv"
#define DEFAULT_BITS_IN_KEYTABLE 28

// Debugging messages
int VERBOSE=0;
int VERY_VERBOSE=0;
// Minimum sentence length to consider
int MINSENL=7;
// Winnowing
int WINK=7;
int WINT=12;
int WINW=(WINT-WINK+1);
// Respect newline as sentence boundary or not
int RESPECT_SENTENCES=0;

string dataDir=DEFAULT_DATA_DIR;
string baseDir=DEFAULT_BASE_DIR;
string filename1="";
string link1="";
string filename2="";
string link2="";
string key="";
string kgram="";
string keyMapFile="";
bool bFlag=false;
int bitsInKeyTable=DEFAULT_BITS_IN_KEYTABLE;
string keyTableFile="";
string keyTableBase="";
int keysForMatch=DEFAULT_KEYS_FOR_MATCH;
bool writeSharedKeys=false;
bool compare=false;
string comparisonFile="comparison";  // will have _long.html or _short.html prepended
bool comparisonStatsOnly=false;
string range;
int rangeStart=0;
int rangeEnd=0;
int selectBits=0;
int selectMatch=0;
string candidatesFile=DEFAULT_CANDIDATES_FILE;

int readOptions(int argc, char* argv[], string argsUsed, string myname, string usage)
{
  int j;
  extern int optind;

  // "+" at start of allArgsUsed tells getopt() to stop at the first non-option
  // argument. optind will then be set to index of next argument in argv.
  // See man 3 getopt.
  string allArgsUsed="+hHvV"+argsUsed;
  char args_str[50];
  size_t k;
  strncpy(args_str,allArgsUsed.c_str(),50);
  while ((j=getopt(argc,argv,args_str))!=-1) {
    switch (j) {
    case 'h': case 'H': case '?':
      writeUsage(args_str,myname,usage);
      exit(1);
    case 'v':
      VERBOSE=1;
      break;
    case 'V':
      VERBOSE=1;
      VERY_VERBOSE=1;
      break;
    case 'd':
      dataDir=(string)optarg;
      break;
    case 'o':
      baseDir=(string)optarg;
      break;
    case 'f':
      filename1=(string)optarg;
      break; 
    case 'F':
      filename2=(string)optarg;
      break; 
    case 'l':
      link1=(string)optarg;
      break; 
    case 'L':
      link2=(string)optarg;
      break; 
    case 'k':
      key=(string)optarg;
      break;
    case 'K':
      kgram=(string)optarg;
      break;
    case 'm':
      keyMapFile=(string)optarg;
      break; 
    case 'b':
      bFlag=true;
      bitsInKeyTable=atoi(optarg);
      break;
    case 't':
      keyTableFile=(string)optarg;
      break;
    case 'T':
      keyTableBase=(string)optarg;
      break;
    case 'n':
      keysForMatch=atoi(optarg);
      break;
    case 'c':
      compare=true;
      break;
    case 'C':
      comparisonFile=(string)optarg;
      break;
    case 's':
      comparisonStatsOnly=true;
      break;
    case 'S':
      RESPECT_SENTENCES=true;
      break;
    case 'p':
      candidatesFile=string(optarg);
      break;
    case 'r':
      range=(string)optarg;
      k=range.find('-');
      if (k==string::npos) {
        // no hyphen, single value
        rangeStart=atoi(range.c_str());
        rangeEnd=rangeStart;
      } else {
        rangeStart=atoi(("0"+range.substr(0,k)).c_str());
        rangeEnd=atoi(("0"+range.substr(k+1)).c_str());
      }
      break;
    case 'w':
      writeSharedKeys=true;
      break;
    case 'x':
      selectBits=atoi(optarg);
      break;
    case 'X':
      selectMatch=atoi(optarg);
      break;
    }
  }

  if (VERBOSE) {
    cout << myname << ": running..." << endl;
    cout << myname << ":      MINSENL=" << MINSENL << "  (minimum sentence length to look at)\n"
         << myname << ":         WINK=" << WINK << "  (kgram length)\n"
         << myname << ":         WINT=" << WINT << "  (guarantee threshold)\n"
         << myname << ":         WINW=" << WINW << "  (window size)\n";
    if (filename1.length()>0) {
      cout << myname << ":    filename1=" << filename1 << endl;
    }
    if (filename2.length()>0) {
      cout << myname << ":    filename2=" << filename2 << endl;
    }
    if (key.length()>0) {
      cout << myname << ":          key=" << key << endl;
    }
    if (keyMapFile.length()>0) {
      cout << myname << ":   keyMapFile=" << keyMapFile << endl;
    }
    if (keyTableFile.length()>0) {
      cout << myname << ": keyTableFile=" << keyTableFile << endl;
    }
    if (keysForMatch>0) {
      cout << myname << ": keysForMatch=" << keysForMatch << endl;
    }
    if (dataDir.length()>0) {
      cout << myname << ":      dataDir=" << dataDir << endl;
    }
    if (baseDir.length()>0) {
      cout << myname << ":      baseDir=" << baseDir << endl;
    }
  }

  return(optind);
}


// Write out the usage text based on the arguments used
// by this program
//
void writeUsage(char* args_str, string myname, string usage)
{
  ostringstream shortArgs;
  ostringstream longArgs;

  cerr << "usage: " << myname;
  char* j=args_str;
  while (*j!='\0') {
    switch(*j) {
    case 'b':
      shortArgs << " -b <#bits>";
      longArgs << "  -b <#bits>         Number of bits to use in KeyTable (default " << DEFAULT_BITS_IN_KEYTABLE << ", 28bits fits in 32bit linux)" << endl;
      break;
    case 'c':
      shortArgs << " -c";
      longArgs << "  -c                 Just do comparison" << endl;
      break;
    case 'C':
      shortArgs << " -C <comp-file>";
      longArgs << "  -C <comp-file>     Comparison file name base for comparison output" << endl;
      break;
    case 'd':
      shortArgs << " -d <datadir>";
      longArgs << "  -d <datadir>       Specify data directory for input files (default " << DEFAULT_DATA_DIR << ")" << endl;
      break;
    case 'f':
      shortArgs << " -f <filename1>";
      longArgs << "  -f <filename1>     Specify normalized txt to examine" << endl;
      break;
    case 'F':
      shortArgs << " -F <filename2>";
      longArgs << "  -F <filename2>     Specify normalized txt to compare filename1 against" << endl;
      break;
    case 'k':
      shortArgs << " -k <key>";
      longArgs << "  -k <key>           Specify kgram key (64bit hex)" << endl;
      break;
    case 'K':
      shortArgs << " -K <kgram>";
      longArgs << "  -K <kgram>         Specify kgram string" << endl;
      break;
    case 'l':
      shortArgs << " -l <link1>";
      longArgs << "  -l <link1>         Link to associate with filename1" << endl;
      break;
    case 'L':
      shortArgs << " -L <link2>";
      longArgs << "  -L <link2>         Link to associate with filename2" << endl;
      break;
    case 'm':
      shortArgs << " -m <KeyMapFile>";
      longArgs << "  -m <KeyMapFile>    Full name of KeyMap file to read" << endl;
      break;
    case 'n':
      shortArgs << " -n <keysForMatch>";
      longArgs << "  -n <keysForMatch>  Number of matching keys for a document match (default " << DEFAULT_KEYS_FOR_MATCH << ")" << endl;
      break;
    case 'o':
      shortArgs << " -o <basedir>";
      longArgs << "  -o <basedir>       Output file base directory (default " << DEFAULT_BASE_DIR << ")" << endl;
      break; 
    case 'p':
      shortArgs << " -p <candidates>";
      longArgs << "  -p <candidates>    File name for candidate overlaps (a DocPairVector, default " << DEFAULT_CANDIDATES_FILE << ")" << endl;
      break;
    case 'r':
      shortArgs << " -r <docid-range>";
      longArgs << "  -r <docid-range>   Add in data from documents in range start-end" << endl;
      break;
    case 's':
      shortArgs << " -s";
      longArgs << "  -s                 Calculate statistics only" << endl;
      break;
    case 'S':
      shortArgs << " -S";
      longArgs << "  -S                 Respect newline as sentence break, winnow within sentences only (default off)" << endl;
      break;
    case 't':
      shortArgs << " -t <KeyTableFile>";
      longArgs << "  -t <KeyTableFile>  Set full name of single KeyTable file" << endl;
      break;
    case 'T':
      shortArgs << " -T <KeyTableBase>";
      longArgs << "  -T <KeyTableBase>  Set base name of KeyTable files (e.g. dir/allkeys for dir/allkeys_#.keytable)" << endl;
      break;
    case 'w':
      shortArgs << " -w";
      longArgs << "  -w                 Write shared keys table (tables23 in KeyTable, index is XX and 6 digit hex num)" << endl;
    case 'x':
      shortArgs << " -x <selectBits>";
      longArgs << "  -x <selectBits>    Number of bits of key to base selection on (must be > #bits from -b)" << endl;
      break;
    case 'X':
      shortArgs << " -X <selectMatch>";
      longArgs << "  -X <selectMatch>   Binary match used on high bits (selectBits..#bits with -x/-b)" << endl;
      break;
    case ':': case '+': case 'v': case 'V': case 'h': case 'H':
      break;
    default:
      shortArgs << " -" << *j << " <?>";
      longArgs << "  -" << *j << " <?>             [Undocumented parameter]" << endl;
    }
    j++;
  }
  
  
  longArgs << "  -v                 Verbose" << endl;
  longArgs << "  -V                 VERY verbose (implies -v)" << endl;
  longArgs << "  -h | -H            This help" << endl;
  cerr << shortArgs.str() << endl << longArgs.str() << endl 
       << usage << endl << endl;
}
