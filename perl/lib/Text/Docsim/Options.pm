package arXiv::Docsim::Options;

=head1 arXiv::Docsim::Options - shared options object for Docsim programs

$Id: Options.pm,v 1.1 2007-08-06 14:04:26 simeon Exp $

./docsim-analyze -d testdata/arxiv-cs-500 -f testdata/arxiv-cs-500/files.txt -b 20

=cut

use strict;

use constant {
  STRING => 1,
  INT => 2,
  NOVAL =>  3
};

my %options = (
  'f' => STRING,
  'h' => NOVAL,
);

=head1 METHODS

=head3 new(%args)

Create object initialized with %args.

=cut

sub new {
  my $that = shift;
  my $class = ref($that) || $that;
  my $self={ @_ };
  bless($self, $class);
  return($self);
}


=head3 as_array(%args)

Returns array of options suitable for use in system() call such as:

  system($BINARY,$options->as_array());

Create object if nescessary passing %args to new() so can be used
as one-of:

  system($BINARY,arXiv::Docsim::Options::as_array('h'=>1));

=cut

sub as_array {
  my $self=shift;
  
  my @opts=();
  foreach my $opt ( keys %$self ) {
    next unless ($options{$opt});
    if ($options{$opt}==NOVAL) {
      push(@opts,"-$opt");
    } else {
      push(@opts,"-$opt",$self->{$opt});
    } 
  }
  return @opts;
}

1;

__END__

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2007 arxiv.org <www-admin@arXiv.org>

This module is free software; you can redistribute it and/or
modify it under the same terms as Perl itself. See L<perlartistic>.

=cut
