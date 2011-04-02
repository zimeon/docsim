// Options.h
// Shared code to read command line arguments in all DocSim codes
// Simeon Warner - 2005-08-29...
//
// $Id: options.h,v 1.3 2011-02-16 22:56:17 simeon Exp $
//
#ifndef __INC_Options
#define __INC_Options 1

#include "definitions.h"

extern int MINSENL;   // minimum sentence length
extern int WINK;
extern int WINT;
extern int WINW;

extern int RESPECT_SENTENCES; 

extern int VERBOSE;
extern int VERY_VERBOSE;

extern string dataDir;
extern string baseDir;
extern string filename1;
extern string link1;
extern string filename2;
extern string link2;
extern string key;
extern string kgram;
extern string keyMapFile;
extern int bitsInKeyTable;
extern string keyTableFile;
extern string keyTableBase;
extern int keysForMatch;
extern bool compare;
extern string comparisonFile;
extern bool comparisonStatsOnly;
extern string range;
extern int rangeStart;
extern int rangeEnd;
extern int selectBits;
extern int selectMatch;

int readOptions(int argc, char* argv[], string argsUsed, string myname, string usage);
void writeUsage(char* args_str, string myname, string usage);

#endif //__INC_Options
