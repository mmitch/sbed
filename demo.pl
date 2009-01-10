#!/usr/bin/perl -w
#
# Honk UI widgets
#
# honk if you write your own widget library because you are too lazy
# to check if an exisiting library already fits your needs
#
# 2009 (c) by Christian Garbs <mitch@cgarbs.de>
# licensed under GNU GPL v2
#

# simple demonstration and test program for Honk classes

use Honk::Form;
use Honk::Label;
use Honk::Textfield;
use Curses;

my $honk = Honk::Form->new(\&quit);

my @labels = (
	      Honk::Label->new(1, 1, 'use [F3] to quit, [enter] to execute'),
	      Honk::Label->new(1, 5, 'value 1:'),
	      Honk::Label->new(1, 6, 'value 2:'),
	      Honk::Label->new(10, 7, '--------'),
	      Honk::Label->new(1, 8, 'result :'),
	      );
$honk->add(@labels);

my $result = Honk::Label->new(10,8, '');
my $val1 = Honk::Textfield->new(11, 5, 7, '');
my $val2 = Honk::Textfield->new(11, 6, 7, '');
$honk->add($val1, $val2, $result);

$honk->mainloop();

sub quit {
    my $key = shift;

    if ($key eq KEY_F(3)) {
	exit 0;
    }

    my $v1 = $val1->text;
    my $v2 = $val2->text;
    $v1 =~ tr/0-9//cd;
    $v2 =~ tr/0-9//cd;
    $v1 = 0 if $v1 eq '';
    $v2 = 0 if $v2 eq '';

    $result->text( sprintf "%8d", 0+$v1+$v2 );
    
}

