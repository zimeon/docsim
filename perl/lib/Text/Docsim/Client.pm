package Text::Docsim::Client;

=head1 NAME

Text::Docsim::Client - client for docsim overlapd server

=head1 SYNPOSIS

 my $filelist=Text::Docsim::FileList->new(file=>$FILELIST,datadir=>$DATADIR);
 my $client=Text::Docsim::Client->new('server'=>$SERVER,'filelist'=>$filelist);
 eval {
   $client->query_file($query_file);
 };


=cut

use strict;
use Carp;
use SOAP::Lite;
#use SOAP::Lite +trace => debug; #for debug SOAP XML, must also turn off use strict

=head2 METHODS

=head3 new(%args)

Create client object. Can set the following parameters:

=cut

sub new {
  my $that = shift;
  my $class = ref($that) || $that;

  my $self={ 'namespace'=>'http://arxiv.org/schemas/overlap.xsd',
             'tag'=>'overlap',
             'url'=>'http://arxiv.org/overlap',
             'server'=>'http://localhost:8081',
             'verbose'=>0,
             'filelist'=>undef,
             'docs'=>undef,
             @_
           };
  bless($self, $class);
  return($self);
}


sub status {
  my $self=shift;

  my $soap = SOAP::Lite->new(uri=>$self->{uri},proxy=>$self->{server});
  $soap->ns($self->{namespace},$self->{tag});
  my $som=$soap->call('overlap:status'=>());
  if ($som->fault()) {
    return("SOAP error: ".$som->faultstring());
  } else {
    my $status=$som->valueof('//statusResponse/status/');
    return($status);
  }
}


=head3 query_file($file)

Read $file and then call $client->query() with the contents. Will
die if there is an error.

=cut


sub query_file {
  my $self=shift;
  my ($query_file)=@_;

  if (open(my $fh,'<',$query_file)) {
    my $nat='';
    while (<$fh>) {
      $nat.=$_;
    }
    close($fh);
    return($self->query($nat));
  } else {
    croak "Failed to open '$query_file' as overlap query file: $!";
  }
}


=head3 query($nat)

Run query against normalized test $nat. Results are put in the 
@$self->{docs} instance variable and the number of results are 
returned.

=cut

sub query {
  my $self=shift;
  my $nat=shift;

  my $soap = SOAP::Lite->new(uri=>$self->{uri},proxy=>$self->{server});
  $soap->ns($self->{namespace},$self->{tag});
  my $som=$soap->call('overlap:overlap'=>SOAP::Data->name('nat'=>$nat));
  if ($som->fault()) {
    croak "SOAP error: ".$som->faultstring();
  } else {
    my $matches=$som->valueof('//overlapResponse/matches/');
    my $docs=$som->valueof('//overlapResponse/docs/');
    # FIXME - overlapd should pass this back as a data structure but
    # for now we just parse the string which is DocPair values separated
    # by linebreaks
    my @lines=split(/\n/,$docs);
    print STDERR __PACKAGE__."::query: docs=".join(' | ',@lines)."\n" if ($self->{verbose});
    print STDERR __PACKAGE__."::query: Got $matches candidate matches\n" if ($self->{verbose});
    if (scalar(@lines)!=$matches) {
      print STDERR __PACKAGE__."::query: Mismatch between claimed matches ($matches) and matches returned (".(scalar(@lines)).").\n";
    }
    $self->{docs}=[];
    foreach my $line (@lines) {
      my ($n,$dummy,$overlap)=split(/\s/,$line);
      my $file='unknown_file';
      if ($self->{filelist}) {
        $file=$self->{filelist}->file($n);
      }
      push(@{$self->{docs}},[$n,$file,$overlap]);
    }
    return(scalar(@{$self->{docs}}));
  }
}

=head3 raw_results

Return a string with the raw results from a query.

=cut

sub raw_results {
  my $self=shift;
  return(join("\n",map {join(" ",@$_)} @{$self->{docs}})."\n");
}


=head2 USE OF LOCAL docsim-overlap to prune

=head3 get_real_matches($query_id,$query_file)

Go through all the candidate matches in @$self->{docs} and prune
any that aren't good matches when tested in a one-to-one
comparison with $query_file (i.e. without truncation of
the kgram hashes at some short bit length). Used check_overlap
to make that comparison.

Returns a hash of new array pointer as the pruned set. Elements within
this point to data inside @$docs.

=cut

sub get_real_matches {
  my $self=shift;
  my ($query_id,$query_file)=@_;

  my $results=Text::Docsim::Results->new();
  $results->query_id($query_id);
  $results->query_file($query_file);

  my $matches=$results->matches;
  foreach my $d (@{$self->{docs}}) {
    my ($n,$file,$overlap)=@$d;
    print STDERR __PACKAGE__.": ($n,$file,$overlap)\n" if ($self->{verbose});
    my ($match,$words1,$match1,$long1,$words2,$match2,$long2)=check_overlap($file,$query_file);
    # Ignore if it isn't really a match
    next if (not $match);
    # Calculate weight
    my $weight=$match1/$words1;
    $weight=($match2/$words2) if (($match2/$words2)>$weight);
    $matches->{$file}={};
    $matches->{$file}{score}=$weight*100.0;
    my $fmt="%d words, %5.1f%s matching, longest block %d words";
    $matches->{$file}{id1}=sprintf($fmt,$words1,(100.0*$match1/$words1),'%',$long1);
    $matches->{$file}{id2}=sprintf($fmt,$words2,(100.0*$match2/$words2),'%',$long2);
  }
  return($results);
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

1;

__END__

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2007-2011 Simeon Warner <simeon@cpan.org>

This module is free software; you can redistribute it and/or
modify it under the same terms as Perl itself. See L<perlartistic>.

=cut

