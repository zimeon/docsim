# Functional tests for docsim-analyze and docsim-compare that
# use KeyTable data from docsim-analyze
# Simeon Warner 2014-08 
use strict;

use FindBin;
use lib "$FindBin::Bin/lib";
use test_config qw($TESTDATA $TESTTMP file_line run);
use Test::More;

plan( tests=> 15 );

{
  my $files = "$TESTDATA/arxiv-publicdomain/files100.txt";
  note("Building test KeyTable from files in $files");
  run("./docsim-analyze -d $TESTDATA/arxiv-publicdomain/ -f $files -b 28 -o $TESTTMP");
  is( $?, 0, "zero signal/exit code" );
  note("Check output files");
  is( (-e "$TESTTMP/warn.txt" and -z _), 1,
      "zero size warn.txt" );
  is( (-e "$TESTTMP/log.txt" and -z _), 1,
      "zero size log.txt");
  is( -s "$TESTTMP/docids.txt", 5698,
      "known size docids.txt");
  is( -s "$TESTTMP/allkeys_1.keytable", 1573144,
      "known size output allkeys_1.keytable");

  note("Use docsim-compare to find an exact match document in the corpus from KeyTable");
  # Cleanup as prep
  my $newdoc_keymap="$TESTTMP/newdoc.keymap";
  my $sharedkeys="$TESTTMP/sharedkeys.keymap";
  my $candidates="$TESTTMP/candidates.dpv";
  unlink($newdoc_keymap) if (-e $newdoc_keymap);
  unlink($sharedkeys) if (-e $sharedkeys);
  unlink($candidates) if (-e $candidates);
  run("./docsim-compare -T $TESTTMP/allkeys -f $TESTDATA/arxiv-publicdomain/1012/1012.5086.txt.gz -b 28");
  is( $?, 0, "zero signal/exit code" );
  is( -s $newdoc_keymap, 32734, "known size $newdoc_keymap");
  is( -s $sharedkeys, 3513866, "known size $sharedkeys" );
  is( -s $candidates, 1518, "known size $candidates" ); 
  is( file_line($candidates,1), "2 9999999 1332",
      "docid 2 overlaps with test document sharing 1332 keys" );

  note("Use docsim-compare to find an exact match document in the corpus from KeyTable, use sentence bournaries");
  # Cleanup as prep
  unlink($newdoc_keymap) if (-e $newdoc_keymap);
  unlink($sharedkeys) if (-e $sharedkeys);
  unlink($candidates) if (-e $candidates);
  run("./docsim-compare -T $TESTTMP/allkeys -f $TESTDATA/arxiv-publicdomain/1012/1012.5086.txt.gz -b 28 -S");
  is( $?, 0, "zero signal/exit code" );
  is( -s $newdoc_keymap, 18460, "known size $newdoc_keymap");
  is( -s $sharedkeys, 3528669, "known size $sharedkeys" );
  is( -s $candidates, 1532, "known size $candidates" ); 
  is( file_line($candidates,1), "1 9999999 569",
      "docid 1 overlaps with test document sharing 569 keys" );

}

# Cleanup
foreach my $file qw(warn.txt log.txt docids.txt allkeys_1.keytable sharedkeys.keymap
                    newdoc.keymap newdoc.indexes candidates.dpv) {
  my $path="$TESTTMP/$file";
  if (-e $path) { 
    unlink($path);
  }
}
