#!/usr/bin/perl

=head1 NAME

docsim-overlap-server-query.pl

=head1 SYNOPSIS

usage: docsim-overlap-server-query.pl [-f file|-n num|-s] [-h] [-v] 

  -s       do status test
  -S num   do speed test with num objects

  -i id    do query for id specified
  -n num   do query for file num in file list (specify -d and -f)
  -d dir   data dir 
  -f file  use <file> to look up files

  -q file  compare <file> against server

  -v       verbose
  -h       this help

=head2 RUNNING A SPEED TEST

To use test data:

 ./docsim-analyze -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt
 ./soap-server/overlapd -b 20 -T /tmp/allkeys

Then in a separate window run the test:

 ./docsim-overlap-server-query.pl -S 1000 -d . -f /tmp/docids.txt

output something like:

 Going to do speed test with 1000 objects...
 Time per item = 0.015s (0 errors ignored)

=cut

use strict;

use lib qw(lib);

use Text::Docsim::Client;
use Text::Docsim::FileList;
use Text::Docsim::Results;
use Getopt::Std;
use Pod::Usage;

my $SERVER='http://localhost:8081';

my %opt=();
(getopts('i:n:f:d:q:sS:whv',\%opt) && !$opt{h}) || pod2usage();

my $html_file='a.html';

my $dir=( $opt{d} || ($opt{f}=~m%^/% ? '' : '.'));

if ($opt{s}) {
  print "Going to issue 100 status queries\n";
  my $client=Text::Docsim::Client->new('proxy'=>$SERVER);
  for (my $j=1; $j<=100; $j++) {
    $client->status();
  }
} elsif ($opt{S}) {
  die "Specify number >=10 with -S" if (not $opt{S}>=10);
  die "Must specify -f with -S" unless ($opt{f});
  print "Going to do speed test with $opt{S} objects...\n";
  my $start=time();
  my $filelist=Text::Docsim::FileList->new(file=>$opt{f},datadir=>$dir);
  my $client=Text::Docsim::Client->new('proxy'=>$SERVER,'filelist'=>$filelist,'verbose'=>$opt{v});
  my $errors=0;
  for (my $j=1; $j<=$opt{S}; $j++) {
    eval {
      print "Testing item $j ..." if ($opt{v});
      my $filenum = (($j-1) % $filelist->size) + 1; # wrap around filelist when $j exceeds size
      my $matches=$client->query_file($filelist->file($filenum));
      print " ($matches matches)\n" if ($opt{v});
    };
    if ($@) {
      print "Error in query (test $j of $opt{S}): $@" if ($opt{v});
      $errors++;
    }
  }
  my $time_per_item=(time()-$start)/$opt{S};
  printf("Time per item = %.3fs (%d errors ignored)\n",$time_per_item,$errors);
} elsif ($opt{n} or $opt{i}) {
  die "Must specify -f with -n/-i" unless ($opt{f});
  my $filelist=Text::Docsim::FileList->new(file=>$opt{f},datadir=>$dir);
  my $query_id;
  my $query_file;
  if ($opt{n}) {
    $query_id=$opt{n};
    $query_file=$filelist->file($opt{n});
  } else {
    $query_id=$opt{i};
    my $file_num;
    ($file_num,$query_file)=$filelist->file_by_id($opt{i});
  }
  my $client=Text::Docsim::Client->new('proxy'=>$SERVER,'filelist'=>$filelist,'verbose'=>$opt{v});
  eval {
    print "Testing $query_file\n";
    $client->query_file($query_file);
  };
  if ($@) {
    die "Error in query: $@";
  }
  my $results=$client->get_real_matches($query_id,$query_file);
  print "Pruned to ".$results->num." real match".($results->num>1?'es':'')."\n";
  if ($opt{w}) {
    open(my $outfh,'>',$html_file) || die "Failed to write to $html_file: $!";
    print $results->as_html;
  } else {
    # Plain text output
    print $results->as_txt;
  }
} elsif ($opt{q}) {
  my $query_id=$opt{q};
  my $filelist;
  if ($opt{f}) {
    $filelist=Text::Docsim::FileList->new(file=>$opt{f},datadir=>$dir);
  }
  my $query_file=$opt{q};
  my $client=Text::Docsim::Client->new('proxy'=>$SERVER,'filelist'=>$filelist,'verbose'=>$opt{v});
  eval {
    print "Testing $query_file\n";
    $client->query_file($query_file);
  };
  if ($@) {
    die "Error in query: $@";
  }
  if ($opt{f}) {
    my $results=$client->get_real_matches($query_id,$query_file);
    print "Pruned to ".$results->num." real match".($results->num>1?'es':'')."\n";
    if ($opt{w}) {
      open(my $outfh,'>',$html_file) || die "Failed to write to $html_file: $!";
      print $results->as_html;
    } else {
      # Plain text output
      print $results->as_txt;
    }
  } else {
    print $client->raw_results;
  }
} else {
  print "Specify -s, -S or -q to do something, -h for help ;-)\n";
}
