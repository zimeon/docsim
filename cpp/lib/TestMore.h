// Very simple test code based on the pattern of Perl's
// Test::More.

#include <string>
using namespace std;

class TestMore
{
private:
  int num;
  int plan_num;
  int pass_num;
  int fail_num;
  bool done_done_testing;  
public: 
  TestMore(int n=0);
  ~TestMore(void);
  void is(string a, string b, string msg="");
  void is(int a, int b, string msg="");
  void contains(string a, string b, string msg="");
  void ok(bool t, string msg="");
  void plan(int num);
  void done_testing();
  void pass(string msg);
  void fail(string msg, string got="", string expected="");
};
