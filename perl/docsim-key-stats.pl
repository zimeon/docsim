#!/usr/bin/perl

=head1 NAME

docsim-key-stats.pl - Create rankings and histogram from kgramkey data

=head1 SYNOPSIS

usage: docsim-key-stats.pl [-t] [-h] [file]

Read input KeyTable (-t) or KeyMap data from [file] or from STDIN.

 -t           expect keytable not keymap file
 -o baseDir   base directory for input and output [default /tmp]
              (expect to find docids.txt here)
 -b bits      number of bits in keytable

 -q           quiet
 -v           verbose
 -h           this help

File formats are:

 Format of keymap files:
 00e2e72d52801e78        [2,2] 2182 2228

 Format of keytable files:
 00e2e72d 2182 2228

=cut

use strict;

use lib qw(lib);

use Text::Docsim::FileList;
use Getopt::Std;
use Pod::Usage;

my %opt;
(getopts('to:b:qhv',\%opt) && !$opt{h}) || pod2usage();

if ($opt{t} and not $opt{b}) {
  die "Must specify number of bits in KeyTable with -b option";
} elsif (not $opt{t} and $opt{b}) {
  die "The -b option is meaningless for a KeyMap";
}

my $base_dir=$opt{o}||'/tmp';

my $DOCIDS=$base_dir.'/docids.txt';
my $HISTOGRAM_BY_DOCUMENTS_FILE=$base_dir.'/key_histogram_by_documents.txt';
my $RANK_BY_DOCUMENTS_FILE=$base_dir.'/key_ranks_by_documents.txt';
my $RANK_BY_OCCURRENCE_FILE=$base_dir.'/key_ranks_by_occurrence.txt';
my $TOP_RANKING_KEYS_TO_FIND=20;
my $TOP_KEYS_FILE=$base_dir.'/key_tops.txt';

my $max_occ=0;
my @occ_hist;
my $max_num_docs=0;
my @num_docs_hist;
my $numKeys=0;
my $line=0;
my %top_ranking_by_doc=();
my %top_ranking_by_occ=();

my $fh=\*STDIN;
my $source='STDIN';
if (scalar(@ARGV)>1) {
  die "Too many arguments";
} elsif (scalar(@ARGV)==1) {
  $source=$ARGV[0];
  open($fh,'<',$source) || die "Failed to open $source for input: $!";
}
print "Reading keys from ".($opt{t}?'KeyTable':'KeyMap')." from $source...\n";
while (<$fh>) { 
  $line++;
  chomp; 
  print "line[$line]: ".substr($_,0,50)."\n" if ($line%100000==0 and not $opt{q});
  if ($opt{t}) {
    # KeyTable format
    if (m/^([X0-9a-f]+)\s+(\d.*)$/) {
      my ($key,$docids)=($1,$2);
      $key=~s/X/0/g; #for sharedkeys
      my @docids=split(/\s/,$docids);
      my $num_docs=scalar(@docids);
      $max_num_docs=$num_docs if ($num_docs>$max_num_docs);
      $num_docs_hist[$num_docs]++;
      accumulate_top_ranking_keys(\%top_ranking_by_doc,$key,$num_docs,\@docids);
      $numKeys++;
    } else {
      warn "line[$line] ignored\n";
    }
  } else {
    # KeyMap format
    if (m/^(\S+)\s+\[(\d+),(\d+)\]\s+(\d.*)$/) {
      my ($key,$occ,$num_docs,$docids)=($1,$2,$3,$4);
      my @docids=split(/\s/,$docids);
      if (scalar(@docids)!=$num_docs) {
        warn "line[$line] mismatch in docids: expected $num_docs but got $docids\n";
      } else {
        $max_occ=$occ if ($occ>$max_occ);
        $occ_hist[$occ]++;
        accumulate_top_ranking_keys(\%top_ranking_by_occ,$key,$occ,\@docids);
        $max_num_docs=$num_docs if ($num_docs>$max_num_docs);
        $num_docs_hist[$num_docs]++;
        accumulate_top_ranking_keys(\%top_ranking_by_doc,$key,$num_docs,\@docids);
        $numKeys++;
      }
    } else {
      warn "line[$line] ignored\n";
    }
  }
}

open(my $histfh,'>',$HISTOGRAM_BY_DOCUMENTS_FILE) || die "Can't open $HISTOGRAM_BY_DOCUMENTS_FILE: $!";
if ($opt{t}) {
  print {$histfh} "# Generated at ".localtime()."\n";
  print {$histfh} "# number_of_docs_sharing_key number_of_keys\n";
  foreach my $j (1..$max_num_docs) { 
    next if (not $num_docs_hist[$j]);
    print {$histfh} "$j\t".$num_docs_hist[$j]."\n";
  }
} else {
  foreach my $j (1..$max_occ) { 
    next if (not $occ_hist[$j] and not $num_docs_hist[$j]);
    print {$histfh} "$j\t".($occ_hist[$j]||'0')."\t".($num_docs_hist[$j]||'0')."\n";
  }
}
close($histfh);
print "Written $HISTOGRAM_BY_DOCUMENTS_FILE\n";

# 
{
  open(my $rankfh,'>',$RANK_BY_DOCUMENTS_FILE) || die "Can't open $RANK_BY_DOCUMENTS_FILE: $!";
  write_rank_data($rankfh, "by documents", \@num_docs_hist, $max_num_docs);
  close($rankfh);
  print "Written $RANK_BY_DOCUMENTS_FILE\n";
}


if (!$opt{t}) {
  open(my $rankfh,'>',$RANK_BY_OCCURRENCE_FILE) || die "Can't open $RANK_BY_OCCURRENCE_FILE: $!";
  write_rank_data($rankfh, "by occurrences", \@occ_hist, $max_occ);
  close($rankfh);
  print "Written $RANK_BY_OCCURRENCE_FILE\n";
}

if (-e $DOCIDS) {
  my $filelist=arXiv::Docsim::FileList->new(file=>$DOCIDS);
  open(my $topfh,'>',$TOP_KEYS_FILE) || die "Can't open $TOP_KEYS_FILE: $!";
  write_info_about_top_ranking_keys($topfh,\%top_ranking_by_doc,$filelist,'by number of documents');
  write_info_about_top_ranking_keys($topfh,\%top_ranking_by_occ,$filelist,'by occurrence') if (!$opt{t});
  close($topfh);
  print "Written $TOP_KEYS_FILE\n";
} else {
  print "Not writing top ranking keys as $DOCIDS doesn't exist\n";
}



# Calculate a frequency (y, col2) vs rank (x, col1) plot from
# a histogram array.
#
sub write_rank_data {
  my ($fh,$str,$hist,$max)=@_;

  print {$fh} "# Generated at ".localtime()."\n";
  print {$fh} "# rank(1 is most shared key) frequency($str)\n";
  print {$fh} "# (only first and last rank show for same frequency, highest rank = number of keys input)\n";
  # Total up histogram and find max value
  my $max_val=scalar(@$hist)-1;
  my $tot=0;
  for (my $j=0; $j<=$max; $j++) { $tot+=$hist->[$j]; }
  #print "tot=$tot\n";
  my $rank=1;

  for (my $freq=$max_val; $freq>0; $freq--) {
    if ($hist->[$freq]>0) {
      print {$fh} "$rank\t$freq\n";
      $rank+=$hist->[$freq];
      print {$fh} ($rank-1)."\t$freq\n" if ($hist->[$freq]>1);
    }
  }
}



=head3 accumulate_top_ranking_keys($top,$freq,$key,$docid)

Using the hash %$top, accumulate a set of top ranking keys, the key that 
occur most freqently. Keep the $TOP_RANKING_KEYS_TO_FIND highest ranking 
keys.

In %$top the hash key is $key and each entry if {freq=>$freq,docs=>$docs}.
 
=cut

sub accumulate_top_ranking_keys {
  my ($top,$key,$freq,$docs)=@_;

  # Find lowest rank in current data
  my $num_keys=0;
  my $lowest_ranking_key=undef;
  foreach my $k (keys %$top) {
    $num_keys++;
    $lowest_ranking_key=$k if (not defined $lowest_ranking_key or $top->{$k}{freq}<$top->{$lowest_ranking_key}{freq});
  }

  # Add new key if we don't have the number we intend to collect
  # of if rank is higher than lowest
  if ($num_keys < $TOP_RANKING_KEYS_TO_FIND) {
    $top->{$key}={freq=>$freq,docs=>$docs};
  } elsif ($freq > $top->{$lowest_ranking_key}{freq}) {
    $top->{$key}={freq=>$freq,docs=>$docs};
    delete $top->{$lowest_ranking_key};
  }
}


=head3 write_info_about_top_ranking_keys($fh,$top,$filelist,$desc)

Write out to filehandle $fh information about the top ranking keys in the
hash %$top. Use 

=cut

sub write_info_about_top_ranking_keys {
  my ($fh,$top,$filelist,$desc)=@_;

  print {$fh} "# Generated at ".localtime()."\n";
  print {$fh} "# Looking at ".scalar(keys %$top)." most frequently occurring ($desc) keys\n";

  my $bits_param=($opt{b} ? "-b ".$opt{b} : '');

  my $rank=0;
  foreach my $key (sort {$top->{$b}{freq}<=>$top->{$a}{freq}} keys %$top) {
    $rank++;
    my $docs=$top->{$key}{docs};
    my $freq=$top->{$key}{freq};
    print {$fh} "\n[$rank] $freq occurrences $desc of key $key (".scalar(@$docs)." documents)\n";   

    my %hits=();
    my $failed=0;
    foreach my $docid (@$docs) {
      my $file=$filelist->file($docid);
      my $cmd="./findkgram -k $key -f $file $bits_param";
      print "Going to do $cmd\n" if ($opt{v});
      my $out=`$cmd`;
      if ($out=~/Found kgram: '(.*)'/) {
        $hits{$1}=[] if (not exists $hits{$1});
        push(@{$hits{$1}},$docid);
      } else {
        $failed++;
        print "Error - failed to get result from '$cmd'\n";
      }
    }

    foreach my $kgram (sort {scalar(@{$hits{$b}})<=>scalar(@{$hits{$a}})} keys %hits) {
      print {$fh} " '$kgram' appears in ".scalar(@{$hits{$kgram}})." docs (e.g. ".$filelist->file($hits{$kgram}[0]).")\n";
    }
    if ($failed>0) {
      print {$fh} "Error - $failed failures (see errors when running)\n";
    }
  }
}


