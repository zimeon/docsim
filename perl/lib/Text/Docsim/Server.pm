package arXiv::Docsim::Server;

=head1 arXiv::Docsim::Server - control agent for docsim overlapd server

$Id: Server.pm,v 1.2 2008-11-19 22:56:27 simeon Exp $

=head1 SYNPOSIS

  use arXiv::Docsim::Server;
  my $server=arXiv::Docsim::Server->start();
  if ($server->is_happy()) {
    # do something
  } else {
    # bwaaa!
  }

=cut

use strict;
use Carp;

=head1 METHODS

=head3 new(%args)

Create server controller object. Can set the following parameters:

=cut

sub new {
  my $that = shift;
  my $class = ref($that) || $that;

  my $self={
             'overlapd'=>pwd().'/soap-server/overlapd',
             @_
           };
  bless($self, $class);
  return($self);
}


=head3 start(%args)

Create server controller object if necessary (parameters passed to new(..)) 
and then attempt to start the server. Will die if server start fails, otherwise
returns controller object so likely the most convenient constructor.

=cut

sub start {
  my $self=shift;
  $self=$self->new(@_) unless (ref($self));
  #
  # Start server

  return($self);
}

1;

__END__

=head1 LICENSE AND COPYRIGHT

Copyright (c) 2007 arxiv.org <www-admin@arXiv.org>

This module is free software; you can redistribute it and/or
modify it under the same terms as Perl itself. See L<perlartistic>.

=cut
