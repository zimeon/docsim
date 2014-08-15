#!/usr/bin/env perl
# Docsim tests to check binaries and libraries are present,
# run me first ;-)
use strict;

use Test::More tests => 9;
use FindBin;
use lib "$FindBin::Bin/lib";

# Perl stuff for testing
BEGIN {
    use_ok( 'test_config', qw($TESTDATA $TESTTMP file_line run) );
}
ok( -d $TESTDATA, 'test data directory exists' );
ok( -d $TESTTMP, 'test tmp directory exists' );

# Docsim binaries
foreach my $bin qw(kgramkey findkgram docsim-overlap docsim-concat 
                   docsim-compare docsim-analyze) {
    ok( (-e $bin && -f _ and -x _), "binary $bin exists and is executable" );
}