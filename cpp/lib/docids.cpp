#include "definitions.h"

string docidhashsetToString(docidhashset& dh)
{
  bool notFirst=0;
  string str;
  char buf[20];
  for (docidhashset::iterator did=dh.begin(); did!=dh.end(); did++) {
    if (notFirst++) str+=",";
    sprintf(buf,"%d",(*did));
    str+=buf;
  }
  return(str);
}
