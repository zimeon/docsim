// Flexible file opening for similarity/plagiarism code. Allows code
// to treat plain and gz files the same. Uses extension typing.
//
// FIXME - Should be rewritten as a nice derived class from ifstream
// instead of this messy function!
//
// $Id: files.cpp,v 1.5 2011-03-03 14:20:24 simeon Exp $

#include "anystream.h"
#include "options.h"
#include <fstream>
#include <gzstream.h>
#include <stdlib.h>
#include <cstring>


// Returns true if *str ends in .gz, false otherwise
//
bool str_ends_in_gz(const char* str)
{
  char* eptr = (char*)str + strlen(str) - 1;
  //cout << "Last chr = '" << *eptr << "'" << endl;
  return( *eptr--=='z' && *eptr--=='g' && *eptr=='.' );
}


// Opens filename, returns stream pointer (test with ptr->good()),
// don't forget to destroy with delete(ptr) after use of stream.
// 
// Will exit with message to STDERR if open fails.
//
// Example of use:
//
//  istream* in=open_plain_or_gz_file(filename);
//  if (!in->good()) {
//    cerr << "Error - failed to read from '" << filename << "'" << endl;
//  } else {
//    string line;
//    while(getline(*in, line)){
//      //do something
//  }
//  delete(in);
//
istream* open_plain_or_gz_file(const char* filename)
{
  istream* in;
  if (str_ends_in_gz(filename)) {
    if (VERY_VERBOSE) {
      cerr << "anystream::open_plain_or_gz_file: reading '" << filename << "' as gzip file" << endl;
    }
    igzstream* fin = new igzstream();
    fin->open(filename);
    in=fin;
  } else {
    if (VERY_VERBOSE) {
      cerr << "anystream::open_plain_or_gz_file: reading '" << filename << "' as plain file" << endl;
    }
    ifstream* pin = new ifstream(filename);
    in=pin;
  }
  if (!in->good()) {
    cerr << "anystream::open_plain_or_gz_file: Error - failed to read from '" << filename << "'\n";
    exit(2);
  }
  return(in);
}


// Simple wrapper to handle string filename
//
istream* open_plain_or_gz_file(string filename)
{
  return(open_plain_or_gz_file(filename.c_str()));
}
