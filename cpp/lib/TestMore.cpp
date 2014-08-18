// Very simple test code based on the pattern of Perl's
// Test::More.

#include "TestMore.h"
#include <stdlib.h>

TestMore::TestMore(int n)
{
  num=0;
  plan_num=n;
  pass_num=0;
  fail_num=0;
  done_done_testing=false;
}

TestMore::~TestMore(void)
{
  // Implicit done_testing() is not already called
  if (not done_done_testing) {
    done_testing();
  }
}

void TestMore::is(string a, string b, string msg)
{
  num++;
  if (a==b) {
    pass(msg);
  } else {
    fail(msg,a,b);
  }
}

void TestMore::ok(bool t, string msg)
{
  num++;
  if (t) {
    pass(msg);
  } else {
    fail(msg,"false","true");
  }
}

void TestMore::plan(int n)
{
  plan_num=n;
}

void TestMore::done_testing()
{
  if (plan_num==0) {
    if (fail_num>0) {
      printf("FAIL %d/%d (no plan)\n",fail_num,pass_num);
    } else {
      printf("OK %d (no plan)\n",pass_num);
    }
  } else if (num==plan_num) {
    if (fail_num>0) {
      printf("FAIL %d/%d\n",fail_num,pass_num);
    } else {
      printf("OK %d\n",pass_num);
    }

  } else {
    if (fail_num>0) {
      printf("FAIL %d/%d (planned %d, ran %d)\n",fail_num,pass_num,plan_num,num);
    } else {
      printf("DUBIOUS %d (planned %d, ran %d)\n",pass_num,plan_num,num);
    }
  }
  done_done_testing=true;
}

void TestMore::pass(string msg)
{
  pass_num++;
  printf("[%03d] OK %s\n",num,msg.c_str());
}

void TestMore::fail(string msg, string got, string expected)
{
  fail_num++;
  printf("[%03d] FAILED %s\n",num,msg.c_str());
  printf("got '%s', expected '%s'\n",got.c_str(),expected.c_str());
}
