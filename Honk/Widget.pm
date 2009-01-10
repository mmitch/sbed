#
# Honk UI widgets
#
# honk if you write your own widget library because you are too lazy
# to check if an exisiting library already fits your needs
#
# 2009 (c) by Christian Garbs <mitch@cgarbs.de>
# licensed under GNU GPL v2
#

package Honk::Widget;
# widget base class
# contains common methods

use strict;
use Curses;

#### constructor

sub new {
    my $class = shift;
    my $self =
    {
	'X' => shift,         # x position
	'Y' => shift,         # y position
	'W' => 0,             # width
	'EDITABLE' => undef,  # editable flag
    };

    bless ($self, $class);

    return $self,
};

#### data access methods

sub x {
    my $self = shift;
    return $self->{X};
}

sub y {
    my $self = shift;
    return $self->{Y};
}

sub w {
    my $self = shift;
    return $self->{W};
}

sub editable {
    my $self = shift;
    return $self->{EDITABLE};
}

#### abstract methods

# put widget on screen
sub draw {
    my $self = shift;
    $self->_bork("Honk::Widget->draw() not overloaded in " . ref($self) . "\n");
}

#### private methods

# restore terminal and die with error
sub _bork {
    my $self = shift;
    endwin();
    die @_;
}

#### end
1;

