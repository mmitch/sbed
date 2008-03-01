#!/usr/bin/perl -w
#
# sbed - screen based editor
# (c) 2008 Christian Garbs <mitch@cgarbs.de>
# 
use strict;
use Curses;

initscr;

my @field = (
	      new_field(1, 10, 4, 18, 0, 0),
	      new_field(1, 10, 6, 18, 0, 0),
	      );

set_field_back($field[0], A_UNDERLINE);
set_field_back($field[1], A_UNDERLINE);

my $form = new_form(@field);
post_form($form);
refresh();

unpost_form($form);
free_form($form);
free_field($_) foreach @field;

endwin;
