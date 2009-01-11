#
# Honk UI widgets
#
# honk if you write your own widget library because you are too lazy
# to check if an exisiting library already fits your needs
#
# 2009 (c) by Christian Garbs <mitch@cgarbs.de>
# licensed under GNU GPL v2
#

package Honk::Form;
# basic form class and main Honk entry point:
# contains widgets, does the screen rendering and calls back on processing

use strict;
use Curses;

#### constructor

sub new {
    my $class = shift;
    my $self =
    {
	'WIN' => new Curses,   # Curses window
	'WIDGETS' => [],       # widget list
	'W' => 80,             # screen width
	'H' => 24,             # screen height
	'CALLBACK' => shift,   # callback method reference
	'X' => 0,              # cursor x
	'Y' => 0,              # cursor y
    };

    bless ($self, $class);

    # initialize Curses
    cbreak();
    noecho();
    $self->{WIN}->keypad(1);

    return $self,
};

#### destructor

sub DESTROY {
    my $self = shift;
    endwin();
}

#### data access methods

sub widgets {
    my $self = shift;
    return $self->{WIDGETS};
}

#### public methods

# remove all widgets
sub clean {
    my $self = shift;
    $self->{WIDGETS} = [];

    $self->{WIN}->clear;
}

# move cursor to first editable widget
sub cursor_home {
    my $self = shift;

    my $start = $self->_find_next(0, 0);
    if ($start) {
	$self->{X} = $start->{X};
	$self->{Y} = $start->{Y};
    }
}

# add one or more widgets
sub add {
    my $self = shift;
    push @{$self->{WIDGETS}}, @_;
}

# input loop until process request
sub mainloop {
    my $self = shift;

    $self->cursor_home;
    
    while (1) {

	# output
	foreach my $widget (@{$self->{WIDGETS}}) {
	    $widget->draw($self->{WIN});
	}
	$self->{WIN}->move($self->{Y}, $self->{X});
	$self->{WIN}->refresh;
	
	
	# input
	my $key;
	while (1) {
	    $key = $self->{WIN}->getch();
	    next if $key eq ERR;

	    # cursor movement
	    if ($key eq KEY_UP) {
		$self->{Y}--;
		$self->{Y} = 0 if $self->{Y} < 0;
		$self->{WIN}->move($self->{Y}, $self->{X});
	    }
	    elsif ($key eq KEY_DOWN) {
		$self->{Y}++;
		$self->{Y} = $self->{H} - 1 unless $self->{Y} < $self->{H};
		$self->{WIN}->move($self->{Y}, $self->{X});
	    }
	    elsif ($key eq KEY_LEFT) {
		$self->{X}--;
		$self->{X} = 0 if $self->{X} < 0;
		$self->{WIN}->move($self->{Y}, $self->{X});
	    }
	    elsif ($key eq KEY_RIGHT) {
		$self->{X}++;
		$self->{X} = $self->{W} - 1 unless $self->{X} < $self->{W};
		$self->{WIN}->move($self->{Y}, $self->{X});
	    }
	    elsif ($key eq "\t") {
		my $offset;
		my $widget = $self->_find($self->{X}, $self->{Y});
		if ($widget) {
		    $offset = $widget->w - ($self->{X} - $widget->x) + 1;
		} else {
		    $offset = 1;
		}
		my $target = $self->_find_next($self->{X}+$offset, $self->{Y});
		if ($target) {
		    $self->{X} = $target->{X};
		    $self->{Y} = $target->{Y};
		    $self->{WIN}->move($self->{Y}, $self->{X});
		}
	    }
	    elsif ($key eq KEY_STAB) {
		my $offset;
		my $widget = $self->_find($self->{X}, $self->{Y});
		if ($widget) {
		    $offset = $widget->x-$self->{X}-1;
		} else {
		    $offset = -1;
		}
		my $target = $self->_find_prev($self->{X}+$offset, $self->{Y});
		if ($target) {
		    $self->{X} = $target->{X};
		    $self->{Y} = $target->{Y};
		    $self->{WIN}->move($self->{Y}, $self->{X});
		}
	    }
	    elsif ($key eq KEY_HOME) {
		my $target = $self->_find_next(0, 0);
		if ($target) {
		    $self->{X} = $target->{X};
		    $self->{Y} = $target->{Y};
		    $self->{WIN}->move($self->{Y}, $self->{X});
		}
	    }

	    # loop breakers
	    last if $key eq KEY_F(3);   # quit
	    last if $key eq "\n";       # process

	    # character input
	    if (length $key == 1 and ord $key > 31 ) {
		my $widget = $self->_find($self->{X}, $self->{Y});
		if (defined $widget and $widget->editable) {
		    my $insert = $widget->addchar($self->{X}, $self->{Y}, $key);
		    if (defined $insert) {
			$widget->draw($self->{WIN});
			$self->{WIN}->refresh;
			$self->{X}++;
			$widget = $self->_find($self->{X}, $self->{Y});
			unless (defined $widget and $widget->editable) {
			    my $target = $self->_find_next($self->{X}, $self->{Y});
			    if ($target) {
				$self->{X} = $target->{X};
				$self->{Y} = $target->{Y};
			    }
			}
			$self->{WIN}->move($self->{Y}, $self->{X});
		    }
		} else {
		    beep;
		}
	    }
	}

	$self->{CALLBACK}($self, $key);

    }
}

#### private methods

# find a widget at position x,y
sub _find {
    my $self = shift;
    my ($x, $y) = @_;

    foreach my $w (@{$self->{WIDGETS}}) {
	if ($w->y == $y) {
	    if ($w->x <= $x and $w->x + $w->w > $x) {
		return $w;
	    }
	}
    }

    return undef;
}

# find an editable widget at position x,y or later
sub _find_next {
    my $self = shift;
    my ($sx, $sy) = (shift, shift);
    my ($x, $y) = ($sx, $sy);

    while (1) {
	
	my $found = $self->_find($x, $y);
	if (defined $found and $found->editable) {
	    return $found;
	}

	$x++;
	if ($x >= $self->{W}) {
	    $x = 0;
	    $y++;
	    if ($y >= $self->{H}) {
		$y = 0;
	    }
	}

	last if $x == $sx and $y == $sy;
    }

    return undef;
}

# find an editable widget at position x,y or sooner
sub _find_prev {
    my $self = shift;
    my ($sx, $sy) = (shift, shift);
    my ($x, $y) = ($sx, $sy);

    while (1) {
	
	my $found = $self->_find($x, $y);
	if (defined $found and $found->editable) {
	    return $found;
	}

	$x--;
	if ($x < 0 ) {
	    $x = $self->{W}-1;
	    $y--;
	    if ($y < 0) {
		$y = $self->{H} - 1;
	    }
	}

	last if $x == $sx and $y == $sy;
    }

    return undef;
}

#### end
1;
