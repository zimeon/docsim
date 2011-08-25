// Header for file handling routines
//
// $Id: files.h,v 1.1 2007/04/23 23:47:08 simeon Exp $

#ifndef __INC_anystream
#define __INC_anystream 1

#include <string>
#include <iostream>
using namespace std;

istream* open_plain_or_gz_file(const char* filename);
istream* open_plain_or_gz_file(string filename);

#endif // __INC_anystream
