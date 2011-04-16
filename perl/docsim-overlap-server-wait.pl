#!/usr/bin/perl

=head1 NAME

docsim-overlap-server-wait.pl - Wait for overlap server to start

=head1 SYNOPSIS

usage: docsim-overlap-server-wait.pl [-t secs] [-s secs] [-h] [-v] 

  -t #     maximum number of seconds to wait for server
  -s #     number of seconds to wait between tries

  -v       verbose
  -q       quiet (not even trail of dots)
  -h       this help

=cut

use strict;

use FindBin;
use lib "$FindBin::Bin/lib";

use Text::Docsim::Client;
use Getopt::Std;
use Pod::Usage;

my $SERVER='http://localhost:8081';
my $DEFAULT_TIME=300; #wait for 300s
my $DEFAULT_SLEEP=2;  #sleep for 2s

my %opt=();
(getopts('s:t:hvq',\%opt) && !$opt{h}) || pod2usage();

my $start_time=time();
my $end_time=$start_time+($opt{t}||300);
$|=1;

if (not $opt{q}) {
  print "Waiting for overlap server to start";
  print "\n" if ($opt{v});
}
while (time()<$end_time) {
  print localtime().": Issuing status query... " if ($opt{v});
  my $client=Text::Docsim::Client->new('proxy'=>$SERVER);
  my $status;
  eval {
    $status=$client->status;
  };
  if ($@) {
    $status="Error: $@";
  }
  print "." if (not $opt{q});
  if ($status eq "I_AM_HAPPY") {
    if ($opt{v} or not $opt{q}) {
      print "OK\n";
    }
    exit(0);
  } else {
    print "not OK ($status)\n" if ($opt{v});
  }
  sleep($opt{s}||$DEFAULT_SLEEP);
}
if ($opt{v}) {
  print localtime().": Time is up, no happy server\n";
} elsif (not $opt{q}) {
  print "FAILED\n";
}
# Return non-zero code for use in Makefile
exit(1);