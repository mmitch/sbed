#!/usr/bin/perl -w
#
# sbed - screen based editor
#
# 2009 (c) by Christian Garbs <mitch@cgarbs.de>
# licensed under GNU GPL v2
#

use Honk::Form;
use Honk::Label;
use Honk::Textfield;
use Curses;

my $editor = Honk::Form->new(\&callback);

my $menu = Honk::Textfield->new(0, 0, 80, '');
my (@linecmd, @lineedit);
foreach my $line (1..24) {
    push @linecmd,
    Honk::Textfield->new(0, $line, 6, sprintf ('%06d', $line));
    push @lineedit,
    Honk::Textfield->new(8, $line, 72, '');
}

$editor->add($menu, @linecmd, @lineedit);

$editor->mainloop();

sub callback {
    my $form = shift;
    my $key = shift;

    # quit?
    if ($key eq KEY_F(3)) {
	exit 0;
    }

}

