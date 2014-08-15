# Simple tests for docsim-overlap
use strict;

use FindBin;
use lib "$FindBin::Bin/lib";
use test_config qw($TESTDATA $TESTTMP file_line file_count_lines run);
use Test::More tests => 6;

{
  note("Compare doc0 with itself - one key, shared");
  run("./docsim-overlap  -c -f $TESTDATA/dummy1/doc0.txt -F $TESTDATA/dummy1/doc0.txt");
  is( file_count_lines("$TESTTMP/sharedkeys.keymap"), 1, "one shared key" );
}

{
  note("Compare doc0 with doc1 - no overlap");
  run("./docsim-overlap  -c -f $TESTDATA/dummy1/doc0.txt -F $TESTDATA/dummy1/doc1.txt");
  is( file_count_lines("$TESTTMP/sharedkeys.keymap"), 0, "no shared keys" );
}

{
  note("Compare doc0 with doc2 - one key overlap");
  run("./docsim-overlap  -c -f $TESTDATA/dummy1/doc0.txt -F $TESTDATA/dummy1/doc2.txt");
  is( file_count_lines("$TESTTMP/sharedkeys.keymap"), 1, "one shared key" );
  is( file_count_lines("$TESTTMP/allkeys.keymap"), 7, "7 keys" );
}

{
  note("Compare doc0 with doc2 - one key overlap. With sentence boundaries");
  run("./docsim-overlap  -c -f $TESTDATA/dummy1/doc0.txt -F $TESTDATA/dummy1/doc2.txt -S");
  is( file_count_lines("$TESTTMP/sharedkeys.keymap"), 1, "one shared key" );
  is( file_count_lines("$TESTTMP/allkeys.keymap"), 1, "1 key" );
}

