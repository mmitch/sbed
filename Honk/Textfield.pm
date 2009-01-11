#
# Honk UI widgets
#
# honk if you write your own widget library because you are too lazy
# to check if an exisiting library already fits your needs
#
# 2009 (c) by Christian Garbs <mitch@cgarbs.de>
# licensed under GNU GPL v2
#

package Honk::Textfield;
# text field class
# a widget that can be edited

use Honk::Widget;
@ISA=qw(Honk::Widget);

use strict;
use Curses;

#### constructor

sub new {
    my $class = shift;
    my $self = $class->SUPER::new(shift, shift);

    $self->{W} = shift;
    $self->{TEXT} = shift;
    $self->{EDITABLE} = 1;

    $self->{TEXT} = '' unless defined $self->{TEXT};
    if (length $self->{TEXT} > $self->{W}) {
	$self->{TEXT} = substr $self->{TEXT}, 0, $self->{W};
    }

    bless ($self, $class);

    return $self,
};

#### data access methods

sub text {
    my $self = shift;
    if (@_) { $self->{TEXT} = shift };
    return $self->{TEXT};
}

#### public methods

# put widget on screen
sub draw {
    my $self = shift;
    my $win = shift;

    $win->attron(A_UNDERLINE);
    $win->addstr($self->{Y},
		 $self->{X},
		 sprintf "%*s", -$self->{W}, $self->{TEXT}
		 );
    $win->attroff(A_UNDERLINE);
}

# enter a character
sub addchar {
    my $self = shift;
    my ($x, $y) = (shift, shift);
    my $char = shift;

    my $pos = $x-$self->{X};

    if ($pos >= 0 and $pos < $self->{W} and $y == $self->{Y}) {
	# support overwrite/insert modes
	if ($pos > length $self->{TEXT}) {
	    $self->{TEXT} .= ' ' x ($pos - length $self->{TEXT});
	}
	substr ($self->{TEXT}, $pos, 1) = $char;
	return 1;
    }

    return 0;
}

#### end
1;
