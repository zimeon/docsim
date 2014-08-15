# Functional tests for docsim-analyze and docsim-compare that
# use KeyTable data from docsim-analyze
# Simeon Warner 2014-08 

use FindBin;
use lib "$FindBin::Bin/lib";
use test_config qw($TESTDATA $TESTTMP file_line run);
use Test::More;

plan( tests=> 11 );

{
  my $files = "$TESTDATA/arxiv-publicdomain/files100.txt";
  note("Building test KeyTable from files in $files");
  run("./docsim-analyze -d $TESTDATA/arxiv-publicdomain/ -f $files -b 28 -o $TESTTMP");
  is( ($? >> 8), 0, "zero exit code" );
  note("Check output files");
  is( (-e "$TESTTMP/warn.txt" and -z _), 1,
      "zero size warn.txt" );
  is( (-e "$TESTTMP/log.txt" and -z _), 1,
      "zero size log.txt");
  is( -s "$TESTTMP/docids.txt", 5698,
      "known size docids.txt");
  is( file_line("$TESTTMP/commonkeys.txt",1),
      "000833734014c287  [3963,16] 13 17 25 45 46 48 53 57 59 63 66 89 92 95 99 100",
      "one common key in 16 documents" );
  is( -s "$TESTTMP/allkeys_1.keytable", 1573144,
      "known size output allkeys_1.keytable");

  note("Use docsim-compare to find an exact match document in the corpus from KeyTable");
  # Cleanup as prep
  my $sharedkeys="$TESTTMP/sharedkeys.txt";
  my $candidates="$TESTTMP/candidates.dpv";
  unlink($sharekeys) if (-e $sharedkeys);
  unlink($candidates) if (-e $candidates);
  run("./docsim-compare -T $TESTTMP/allkeys -f $TESTDATA/arxiv-publicdomain/1012/1012.5086.txt.gz -b 28");
  is( ($? >> 8), 0, "zero exit code" );
  is( -s "$TESTTMP/newdoc.keymap", 32734,
      "known size newdoc.keymap");
  is( -s $sharedkeys, 32752, "known size sharedkeys.txt" );
  is( -s $candidates, 15, "known size candidates.dpv" ); 
  is( file_line($candidates,1), "1 9999999 1259",
      "docid 1 overlaps with test document sharing 1259 keys" )
}
