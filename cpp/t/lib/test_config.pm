package test_config;

use Test::More;

use base qw(Exporter);
our @EXPORT_OK = qw($TESTTMP $TESTDATA file_line run);

our $TESTTMP = '/tmp';
our $TESTDATA = '../testdata';

=head3 run(@_)

Simple wrapper around system() that also shows command as
a note().

=cut

sub run {
    note(join(' ','RUNNING:',@_));
    return system(@_);
}


=head3 file_line($file,$line)

Return line number $line from $file. Will return undef for
any error case but otherwise be silent.

=cut

sub file_line {
    my ($file, $line) = @_;
    if (open(my $fh, '<', $file)) {
	my $n=0;
	while (<$fh>) {
	    $n++;
	    if ($n==$line) {
		close($fh);
		chomp;
		return($_);
	    }
        }
    }
    return undef;
}

1;

