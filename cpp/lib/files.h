// Header for file handling routines
//
// $Id: files.h,v 1.1 2007/04/23 23:47:08 simeon Exp $

#ifndef __INC_files
#define __INC_files 1

#include <string>
#include <iostream>
using namespace std;

string prependPath(string path, string filename);

bool readLine(istream &fin, char* buf, int bufSize);
char* readLine(istream &fin);

#endif // __INC_files
