package Text::Docsim::FileList;

=head1 NAME

Text::Docsim::FileList - control agent for docsim overlapd server

=head1 SYNPOSIS

  use Text::Docsim::FileList;
  my $filelist=Text::Docsim::FileList->new(file=>$listfile);
  my $file=$filelist->file($num);

=cut

use strict;
use Carp;

=head1 METHODS

=head3 new(%args)

Create server controller object. Can set the following parameters:

  file - file containing the file list
  datadir - directory that files names in file list are relative to
  verbose - try for verbose output to STDERR

  files - array of files (will be set by loading file)

=cut

sub new {
  my $that = shift;
  my $class = ref($that) || $that;

  my $self={ file=>undef,
             datadir=>undef,
             verbose=>0,
             files=>[],
             @_
           };
  bless($self, $class);
  if ($self->{file}) {
    $self->load_list();
  }
  return($self);
}


=head3 load_list()

The file is simply a list of numbers (optional) and file names. We
read them and number them from 1 up, the number on each line should
match the count of lines so far.

=cut

sub load_list {
  my $self=shift;

  my $base='';
  if ($self->{datadir}) {
    $base=$self->{datadir};
    $base=~s%(/*)$%/%; #always end in /
  }
  if (open(my $fh, '<', $self->{file})) {
    my $n=0;
    while (my $line=<$fh>) {
      $n++;
      chomp($line);
      my $file;
      if ($line!~/\s/) {
        # Just a file name
        $file=$line;
      } elsif ($line=~/\d\s+\S/) {
        # Must have numbers and file names
        my $num;
        ($num,$file)=split(/\s/,$_);
        if ($num!=$n) {
          die "Error in file list, got number $num when expecting $n on line $n in '".$self->{file}."\n";
        }
      } else {
        die "Error in file list (bad line $n): $line\n"; 
      }
      $self->{files}[$n]="$base$file";
    }
    warn __PACKAGE__,": Loaded $n files\n" if ($self->{verbose});
  } else {
    die "Can't read list of files/numbers from '".$self->{file}."': $!";
  }
}


=head3 file($num)

Lookup file name by number

=cut

sub file {
  my $self=shift;
  my $n=shift;
  return($self->{files}[$n] || "failed_lookup_doc_$n");
}


=head3 size

Return number of file names, which have numbers 1...size.

=cut

sub size {
  my $self=shift;
  return(scalar(@{$self->{files}})-1); #-1 because we don't use element 0
}

1;

__END__

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2007-2011 Simeon Warner <simeon@cpan.org>

This module is free software; you can redistribute it and/or
modify it under the same terms as Perl itself. See L<perlartistic>.

=cut
