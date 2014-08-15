
#include <stdio.h>
#include "pstats.h"

int main() {
  printf("Getting pstats 10 times with 1s delay:\n");
  for (int j=1; j<11; j++) {
    string pstats=get_pstats_string();
    printf("get_pstats_string[%02d]: %s\n",j,pstats.c_str());
    sleep(1);
  }
}
