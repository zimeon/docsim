// Get process statistics from Linux using data in /proc
//
// Code based on that from Trent (tapted) at 
// http://www.linuxforums.org/forum/linux-programming-scripting/11703-c-function-returns-cpu-memory-usage.html
//
// $Id: pstats.cpp,v 1.2 2011-02-09 21:07:14 simeon Exp $

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sstream>
#include "pstats.h"


// Return string describing memory usage
//
// Numbers is /proc/PID/statm are in 4096 byte pages
//
string get_pstats_string(void) {
  char buf[30];
  unsigned int pid=(unsigned)getpid();
  snprintf(buf, 30, "/proc/%u/statm", pid);
  FILE* pf = fopen(buf, "r");
  ostringstream sout;
  sout.precision(4);
  if (pf) {
    unsigned size; //      total program size
    unsigned resident;//   resident set size
    unsigned share;//      shared pages
    unsigned text;//       text (code)
    unsigned lib;//        library
    unsigned data;//       data/stack
    int ret=fscanf(pf, "%u %u %u %u %u %u", &size, &resident, &share, &text, &lib, &data);
    fclose(pf);

    if (ret==6) {    
      // Write to string stream an get string
      sout << "data=" << (data/256.0)
           << "MB: share=" << (share/256.0)
           << "MB code=" << (text/256.0)
           << "MB lib=" << (lib/256.0)
           << "MB  (total=" << (size/256.0)
           << "MB, pid " << pid << ")";  
    } else {
      sout << "Failed to read/parse process stats for pid " << pid;
    }
  } else {
    sout << "Failed to read process stats for pid " << pid;
  }
  return(sout.str());
}
