package Text::Docsim::Results;

=head1 NAME

Text::Docsim::Results - A set of results from overlap search

=head1 SYNOPSIS

FIXME - Has some arXiv specific code to do with ids

  use Text::Docsim::Results;

  my $docsim_html=Text::Docsim::HTML->new();
  $docsim_html->write_comparison();

=cut

use Moose;

=head1 METHODS

=head3 new()

Uses L<Moose> to provide new() and to provide accessors for instance 
variables which are:

  query_id
  query_file
  matches
  overlap_url

=cut

has 'query_id' =>
  ( 'is' => 'rw',
    'isa' => 'Str',
    'default' => 'NONE' );

has 'query_file' =>
  ( 'is' => 'rw',
    'isa' => 'Str',
    'default' => 'NONE' );

has 'matches' => 
  ( 'is' => 'rw',
    'isa' => 'HashRef',
    'default' => sub { {} } );

has 'overlap_url' =>
  ( 'is' => 'rw',
    'isa' => 'Str',
    'default' => 'http://export.arxiv.org/overlap' );


=head3 $results->sort_by_score()

Return sorted list of the keys of %$matches, sorted by 
$self->matches{key}{score} order.

=cut

sub sort_by_score {
  my $self=shift;
  my $matches=$self->matches;
  return(sort {$matches->{$b}{score}<=>$matches->{$a}{score}} keys %$matches);
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


=head3 as_html($query_id,$query_file,$matches)

Return a string that is the body of a results page for a query of 
$query_id,$query_file which returned $matches.

=cut

sub as_html {
  my $self=shift;

  my $str.="<h1>Overlaps with ".$self->query_id."</h1>\n";

  my $num_matches=scalar(keys %{$self->matches});
  if ($num_matches==0) {
    $str.="<p>No overlapping documents found</p>\n";
    return();
  }

  $str.="<p>Found $num_matches document".($num_matches>1?'s':'').
        " overlapping with ".$self->query_id." by more than XX kgrams.</p>\n";
  $str.="<table border=\"2\">\n";
  $str.="<tr><th>Overlap</th><th>Document</th><th>Details</th></tr>\n";
  foreach my $file ($self->sort_by_score) {
    $str.="<tr>\n";
    $str.=" <td>".$self->matches->{$file}{score}."</td>\n";
    $str.=" <td><a href=\"".$self->overlap_url.
          "?id1=$file&id2=".$self->query_id."\">".
          _guess_id_from_file($file)."<br />vs.<br />".
          $self->query_id."</a></td>\n";
    $str.=" <td>".$self->matches->{$file}{id1}."<br />&nbsp;<br />".
          $self->matches->{$file}{id2}."</td>\n";
    $str.="</tr>\n";
  }
  $str.="</table>\n";
  return($str);
}


=head3 as_txt()

Return summary as plain text string.

=cut

sub as_txt {
  my $self=shift;

  my $txt='';
  my $matches=$self->matches;
  foreach my $file ($self->sort_by_score) {
    $txt.=sprintf("%6.2f%s  ",$matches->{$file}{score},'%').$file."\n";
    $txt.="         (".$matches->{$file}{id1}.")\n";
    $txt.="         vs (".$matches->{$file}{id2}.")\n";
    $txt.="         http://export.arxiv.org/overlap?id1=".
          _guess_id_from_file($file).
          "&id2=".$self->query_id."\n";
  }
  return($txt);
}


=head3 num()

Return number of matches in result set.

=cut

sub num {
  my $self=shift;
  return(scalar(keys %{$self->matches}));
}


=head1 HELPER SUBROUTINES

=head3 _guess_id_from_file

Use a bunch of lax heuristics to get an id from a filename. Be flexible
so that we can test this stuff with various directories etc.

=cut

sub _guess_id_from_file {
  my ($file)=@_;
  my $id=$file;
  if ($file=~m%/([a-z-]+)/\d{4}/(\d{7}(v\d+)?)\.%) {
    $id="arXiv:$1/$2";
  } elsif ($file=~m%/arxiv/\d{4}/(\d{4}\.\d{4}(v\d+)?)\.%) {
    $id="arXiv:$1";
  }
  return($id);
}

1;

__END__

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2007-2011 Simeon Warner <simeon@cpan.org>

This module is free software; you can redistribute it and/or
modify it under the same terms as Perl itself. See L<perlartistic>.

=cut
