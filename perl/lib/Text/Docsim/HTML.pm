package Text::Docsim::HTML;

=head1 NAME

Text::Docsim::HTML - Support for writing HTML pages for document similarity code

=head1 SYNOPSIS

  use Text::Docsim::HTML;

  my $docsim_html=Text::Docsim::HTML->new();
  $docsim_html->write_comparison();

=cut

use Text::Docsim::Client;
use Text::Docsim::FileList;
use Getopt::Std;
use Moose;

has 'overlap_url' =>
  ( 'is' => 'rw',
    'isa' => 'Str',
    'default' => 'http://export.arxiv.org/overlap' );
    $client->status();
  }
} elsif ($opt{q}) {
  my $query_id=$opt{q};
  my $query_file=$opt{q};
  my $filelist=Text::Docsim::FileList->new(file=>$opt{f},datadir=>$opt{d});
  if (open(my $fh,'<',$query_file)) {
    my $nat='';
    while (<$fh>) {
      $nat.=$_;
    }
    print "Issuing request with ".length($nat)." bytes from $opt{d}\n";
    my $client=Text::Docsim::Client->new('proxy'=>$SERVER,'filelist'=>$filelist);
    my ($num_candidates,$docs)=$client->query($nat);
    print "Got $num_candidates candidates\n";

    my $matches=get_real_matches($query_file,$docs);
    print "Pruned to ".scalar(keys %$matches)." real matches\n";
    if ($opt{w}) {
      open(STDOUT,'>','a.html');
      write_html_results_page($query_id,$query_file,$matches);
    } else {
      # Plain text output
      foreach my $file (sort {$matches->{$b}[0]<=>$matches->{$a}[0]} keys %$matches) {
        printf("%6.2f%s  %s  %s\n",$matches->{$file}[0],'%',$matches->{$file}[1],$file);
        print "http://export.arxiv.org/overlap?id1=".$matches->{$file}[0]."&id2=$query_id\n";
      }
    }
  } else {
    die "Failed to read from $opt{f}: $!"; 
  }
} else {
  print "Specify -s or -q to do something ;-)";
}


=head3 get_real_matches($query_file,$docs)

Go through all the candidate matches in @$docs and prune
any that aren't good matches when tested in a one-to-one
comparison with $query_file (i.e. without truncation of
the kgram hashes at some short bit length). Used check_overlap
to make that comparison.

Returns a hash of new array pointer as the pruned set. Elements within
this point to data inside @$docs.

=cut

sub get_real_matches {
  my ($query_file,$docs)=@_;

  my %matches=();
  foreach my $d (@$docs) {
    my ($file,$overlap)=@$d;
    print "($file,$overlap)\n";
    my ($match,$words1,$match1,$long1,$words2,$match2,$long2)=check_overlap($file,$opt{q});
    # Ignore if it isn't really a match
    next if (not $match);
    # Calculate weight
    my $weight=$match1/$words1;
    $weight=($match2/$words2) if (($match2/$words2)>$weight);
    $matches{$file}={};
    $matches{$file}{score}=$weight*100.0;
    my $fmt="%d words, %5.1f%s matching, longest block %d words";
    $matches{$file}{id1}=sprintf($fmt,$words1,(100.0*$match1/$words1),'%',$long1);
    $matches{$file}{id2}=sprintf($fmt,$words2,(100.0*$match2/$words2),'%',$long2);
  }
  return(\%matches);
}


=head1 check_overlap($file1,$file2)

Check overlap of two files, $file1 and $file2, using docsim-overlap.

=cut

sub check_overlap {
  my ($file1,$file2)=@_;

  my $out=`./docsim-overlap -f $file1 -F $file2 -s`;
  #print $out;
  my ($words1,$match1,$long1,$words2,$match2,$long2)=$out=~/#STATS#:\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)/;
  my $match=($match1+$match2);
  return($match,$words1,$match1,$long1,$words2,$match2,$long2);
}



=head3 matching_files($matches)

Return sorted list of the keys of %$matches, sorted by $matches->{key}{score}
order.

=cut

sub matching_files {
  my ($matches)=@_;
  return(sort {$matches->{$b}{score}<=>$matches->{$a}{score}} keys %$matches);
}


=head3 write_results($query_id,$query_file,$matches)

Write the body of a results page for a query of $query_id,$query_file
which returned $matches.

=cut

sub write_results {
  my ($query_id,$query_file,$matches) = @_;

  print "<h1>Overlaps with $query_id</h1>\n";

  my $num_matches=scalar(keys %$matches);
  if ($num_matches==0) {
    print "<p>No overlapping documents found</p>\n";
    return();
  }

  print "<p>Found $num_matches document".($num_matches>1?'s':'')." overlapping with $query_id by
more than XX kgrams.</p>\n";
  print "<table border=\"2\">\n";
  print "<tr><th>Overlap</th><th>Document</th><th>Details</th></tr>\n";
  foreach my $file (matching_files($matches)) {
    print "<tr>\n";
    print " <td>".$matches->{$file}{score}."</td>\n";
    print " <td><a href=\"$OVERLAP_BASEURL?id1=$file&id2=$query_id\">".
          guess_id_from_file($file)."<br />vs.<br />$query_id</a></td>\n";
    print " <td>".$matches->{$file}{id1}."<br />&nbsp;<br />".$matches->{$file}{id2}."</td>\n";
    print "</tr>\n";
  }
  print "</table>\n";

}


=head3 guess_id_from_file

Use a bunch of lax heuristics to get an id from a filename. Be flexible
so that we can test this stuff with various directories etc.

=cut

sub guess_id_from_file {
  my ($file)=@_;
  my $id=$file;
  if ($file=~m%/([a-z-]+)/\d{4}/(\d{7})(v\d+)?\.%) {
    $id="Text:$1/$2$3";
  } elsif ($file=~m%/arxiv/\d{4}/(\d{4}\.\d{4})(v\d+)?\.%) {
    $id="Text:$1$2";
  }
  return($id);
}

__END__

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2007-2011 Simeon Warner <simeon@cpan.org>

This module is free software; you can redistribute it and/or
modify it under the same terms as Perl itself. See L<perlartistic>.

=cut
