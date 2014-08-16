// Very simple test code based on the pattern of Perl's
// Test::More.

#include <string>
using namespace std;

void is(string a, string b, string msg="");
void plan(int num);
void done_testing();
