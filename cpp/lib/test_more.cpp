// Very simple test code based on the pattern of Perl's
// Test::More.

#include "test_more.h"
#include <stdlib.h>

int _num=0;
int _plan_num=0;
int _pass_num=0;
int _fail_num=0;

void _pass(string msg)
{
  _pass_num++;
  printf("[%03d] OK %s\n",_num,msg.c_str());
}

void _fail(string msg, string got, string expected)
{
  _fail_num++;
  printf("[%03d] FAILED %s\n",_num,msg.c_str());
  printf("got '%s', expected '%s'\n",got.c_str(),expected.c_str());
}

void is(string a, string b, string msg)
{
  _num++;
  if (a==b) {
    _pass(msg);
  } else {
    _fail(msg,a,b);
  }
}

void plan(int num)
{
  _plan_num=num;
}

void done_testing()
{
  if (_plan_num==0) {
    if (_fail_num>0) {
      printf("FAIL %d/%d (no plan)\n",_fail_num,_pass_num);
    } else {
      printf("OK %d (no plan)\n",_pass_num);
    }
  } else if (_num==_plan_num) {
    if (_fail_num>0) {
      printf("FAIL %d/%d\n",_fail_num,_pass_num);
    } else {
      printf("OK %d\n",_pass_num);
    }

  } else {
    if (_fail_num>0) {
      printf("FAIL %d/%d (planned %d, ran %d)\n",_fail_num,_pass_num,_plan_num,_num);
    } else {
      printf("DUBIOUS %d (planned %d, ran %d)\n",_pass_num,_plan_num,_num);
    }
  }
}
