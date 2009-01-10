#
# Honk UI widgets
#
# honk if you write your own widget library because you are too lazy
# to check if an exisiting library already fits your needs
#
# 2009 (c) by Christian Garbs <mitch@cgarbs.de>
# licensed under GNU GPL v2
#

package Honk::Label;
# label class
# a static widget showing a text

use Honk::Widget;
@ISA=qw(Honk::Widget);

use strict;
use Curses;

#### constructor

sub new {
    my $class = shift;
    my $self = $class->SUPER::new(shift, shift);

    $self->{TEXT} = shift;
    $self->{W} = length $self->{TEXT};
    $self->{EDITABLE} = 0;

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

    $win->addstr($self->{Y}, $self->{X}, $self->{TEXT});
}

#### end
1;

