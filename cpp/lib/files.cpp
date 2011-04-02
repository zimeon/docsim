// File handling routines for similarity/plagiarism code
//
// The similarity code is written around the notion of analysis
// over sentences. In this module we implement a switch based on 
// the configuration RESPECT_SENTENCES (boolean) where, if true,
// linebreaks are interpretted as sentences. If false, the whole
// file is treated as one sentence/line and returned by readLine.
//
// $Id: files.cpp,v 1.5 2011-03-03 14:20:24 simeon Exp $

#include "files.h"
#include "definitions.h"
#include "options.h"
#include <stdio.h>   // for EOF

// Preped the supplied path to any non-absolute filename
// supplied. Returns string.
//
string prependPath(string path, string filename) {
  if (path=="" || filename[0]=='/' || filename[0]=='.') {
    return(filename);
  } else {
    return(path+fsep+filename);
  }
}


// Safe version of getline that will read a line or file up 
// to the specified buffer size. Return value is true unless the
// end-of-file is reached (i.e. to use with while () ). The
// buf array is always terminated with '\0'.
//
// In the case that the line or file is longer than bufSize, 
// bufSize-1 characters are read. The next read will continue reading 
// the rest of the line.
//
bool readLine(istream &fin, char* buf, int bufSize) {
  bool notEOF=true;
  int j;
  for (j=0; j<bufSize; j++) {
    int ch=fin.get();
    if (ch=='\n') {
      if (RESPECT_SENTENCES) {
        // stop for end of line which indicates end of sentence
        buf[j]='\0';
        break;
      } else {
        // ignore end of line, treat as space
        ch=' ';
      }
    } else if (ch==EOF) {
      if (j>0 && buf[j-1]==' ') {
        // remove previous char if it was a space
        j--;
      }
      buf[j]='\0';
      notEOF=false;
      break;
    }
    buf[j]=ch;
  }
  if (j>=bufSize) {
    // Haven't got to rest end of line. Push back last 
    // char and then we just start reading again on the 
    // next call.
    fin.putback(buf[bufSize-1]);
    buf[bufSize-1]='\0';
    cerr << "Warning - split line that exceeded buffer size (" << bufSize << " chars)" << endl;
  } 
  if (j>0 && !notEOF) {
    // Read out the current buffer now. Put EOF back on fin so we'll get it next time
    fin.putback(EOF);
    notEOF=true;
  }
  return(notEOF);
}


// Version of readLine that uses buffer space allocated here.
//
char buf[FILE_BUFFER_SIZE];
//
char* readLine(istream &fin) {
  if (readLine(fin, buf, FILE_BUFFER_SIZE)) {
    return(buf);
  } else {
    return((char*)NULL);
  }
}
