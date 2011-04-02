
#include <stdio.h>
#include "pstats.h"

char str[80];

int main() {
  printf("The printf way:\n\n");
  for (int j=1; j<11; j++) {
    get_pstats_string(str,80);
    printf("get_pstats_string[%02d]: %s\n",j,str);
  }

  cout << endl << endl << "The cout way:" << endl << endl;
  for (int j=1; j<11; j++) {
    cout << "write_pstats_string[" << j << "]: ";
    write_pstats_string(cout);
    cout << endl;
  }
}
