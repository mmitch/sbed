/*
 * sbed - screen based editor
 * (c) 2008 Christian Garbs <mitch@cgarbs.de>
 *
 * Basic design and madtty inclusion is highly inspired by the dvtm
 * and reuses some code of it which is mostly
 *
 * (c) 2007-2008 Marc Andre Tanner <mat at brain-dump dot org>
 *
 * See LICENSE for details.
 */

#define _GNU_SOURCE
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <ncurses.h>
#include <stdio.h>
#include <signal.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include "madtty.h"

#define ALT(k)      ((k) + (161 - 'a'))
#ifndef CTRL
  #define CTRL(k)   ((k) & 0x1F)
#endif
#define CTRL_ALT(k) ((k) + (129 - 'a'))

typedef struct {
        void (*cmd)(void);
} Action;

typedef struct {
        unsigned int code;
        Action action;
} Key;

typedef struct Label Label;
struct Label {
	unsigned int x;
	unsigned int y;
	unsigned int length;
	const char *text;
	Label *next;
};

typedef struct Field Field;
struct Field {
	unsigned int x;
	unsigned int y;
	unsigned int length;
	char *content;
	Field *next;
};

#define COLOR(fg, bg) madtty_color_pair(fg, bg)
#define countof(arr) (sizeof (arr) / sizeof((arr)[0]))
#define sstrlen(str) (sizeof (str) - 1)
#define max(x, y) ((x) > (y) ? (x) : (y))

#ifdef NDEBUG
 #define debug(format, args...)
#else
 #define debug eprint
#endif

/* commands for use by keybindings */
void cursor_advance();
void cursor_up();
void cursor_down();
void cursor_left();
void cursor_right();
void next_field();
void next_line();
void quit();

/* internal functions */
void draw_all();

unsigned int bh = 1, by, waw, wah, wax, way;

#include "config.h"

const char *shell;
bool need_screen_resize = true;
int width, height;
unsigned int cx = 0, cy = 0;
bool running = true;
Label *labels;
Field *fields;

madtty_t *term;

void
eprint(const char *errstr, ...) {
	va_list ap;
	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
}

void
sigchld_handler(int sig){
	int child_status;
	signal(SIGCHLD, sigchld_handler);
	pid_t pid = wait(&child_status);
	debug("child with pid %d died\n", pid);
}

void
sigwinch_handler(int sig){
	struct winsize ws;
	signal(SIGWINCH, sigwinch_handler);
	if(ioctl(0, TIOCGWINSZ, &ws) == -1)
		return;

	width = ws.ws_col;
	height = ws.ws_row;
	need_screen_resize = true;
}

void
sigterm_handler(int sig){
	running = false;
}

void
resize_screen(){
	debug("resize_screen()\n");
	if(need_screen_resize){
		debug("resize_screen(), w: %d h: %d\n", width, height);
	#if defined(__OpenBSD__) || defined(__NetBSD__)
		resizeterm(height, width);
	#else
		resize_term(height, width);
	#endif
		wresize(stdscr, height, width);
		wrefresh(curscr);
		draw_all();
	}
	need_screen_resize = false;
}

Key*
find_key(unsigned int code){
        unsigned int i;
        for(i = 0; i < countof(keys); i++){
        	if(keys[i].code == code)
        	return &keys[i];
	}
	return NULL;
}

Field*
find_field(unsigned int x, unsigned int y){
	Field *i;
	for (i = fields; i != NULL; i = i->next)
		if ((y == i->y) && (x >= i->x) && (x < (i->x + i->length)))
				return i;
	return NULL;
}

Label*
find_label(unsigned int x, unsigned int y){
	Label *i;
	for (i = labels; i != NULL; i = i->next)
		if ((y == i->y) && (x >= i->x) && (x < (i->x + i->length)))
				return i;
	return NULL;
}

void
addField(Field *new){
	new->next = fields;
	fields = new;
}

void
addLabel(Label *new){
	new->next = labels;
	labels = new;
}

void
setup(){
	int i;
	mmask_t mask;
	if(!(shell = getenv("SHELL")))
		shell = "/bin/sh";
	setlocale(LC_CTYPE,"");
	initscr();
	start_color();
	noecho();
   	keypad(stdscr, TRUE);
	raw();
	madtty_init_colors();
	madtty_init_vt100_graphics();
	getmaxyx(stdscr, height, width);
        term = madtty_create(height, width);
	resize_screen();
	signal(SIGWINCH, sigwinch_handler);
	signal(SIGCHLD, sigchld_handler);
	signal(SIGTERM, sigterm_handler);
}

void
cleanup(){
	endwin();
}

void
update_cursor(){
	move(cy,cx);
	refresh();
}

void
enter_character(unsigned int code){
	addch(code);
	cursor_advance();
}

void
cursor_advance(){
	cx++;
	if (cx >= width){
		cx = 0;
		cursor_down();
	}
}

void
cursor_up(){
	if (cy < 1)
		cy = height;
	cy--;
	update_cursor();
}

void
cursor_down(){
	cy++;
	if (cy >= height)
		cy = 0;
	update_cursor();
}

void
cursor_left(){
	if (cx < 1)
		cx = width;
	cx--;
	update_cursor();
}

void
cursor_right(){
	cx++;
	if (cx >= width)
		cx = 0;
	update_cursor();
}

void
next_field(){
	next_line();
}

void
next_line(){
	cx = 0;
	cursor_down();
}

void
quit(){
	running = false;
}

void
usage(){
	cleanup();
	eprint("usage: sbed\n");
	exit(EXIT_FAILURE);
}

void
draw_fields(){
	Field *f;
	unsigned int i;
	for (f = fields; f != NULL; f = f->next){
		move(f->y, f->x);
		attrset(ATTR_INPUT);
		addstr(f->content);
		attrset(ATTR_INPUT_EMPTY);
		for (i = strlen(f->content); i < f->length; i++)
			addch(' ');
	}
}

void
draw_labels(){
	Label *l;
	unsigned int i;
	attrset(ATTR_LABEL);
	for (l = labels; l != NULL; l = l->next){
		move(l->y, l->x);
		addstr(l->text);
	}
}

void
draw_all(){
	draw_fields();
	draw_labels();
	refresh();
}

int
main(int argc, char *argv[]){
	setup();
	wrefresh(curscr);
	
	
	/* test entries */
	Label *label = calloc(sizeof(Label), 1);
	label->x = 3;
	label->y = 2;
	label->text = "Test label";
	label->length = strlen(label->text);
	addLabel(label);

	Field *field = calloc(sizeof(Field), 1);
	field->x = 1;
	field->y = 4;
	field->length = 20;
	field->content = calloc(sizeof(char), field->length+1);
	sprintf(field->content, "eins");
	addField(field);
	
	field = calloc(sizeof(Field), 1);
	field->x = 1;
	field->y = 6;
	field->length = 20;
	field->content = calloc(sizeof(char), field->length+1);
	sprintf(field->content, "zwei");
	addField(field);

	draw_all();
	
	while(running){
		int r, nfds = 0;
		fd_set rd;

		if(need_screen_resize)
			resize_screen();

		FD_ZERO(&rd);
		FD_SET(STDIN_FILENO, &rd);

		r = select(2, &rd, NULL, NULL, NULL);

		if(r == -1 && errno == EINTR)
			continue;

		if(r < 0){
			perror("select()");
			exit(EXIT_FAILURE);
		}

		if(FD_ISSET(STDIN_FILENO, &rd)){
			int code = getch();
			Key *key;
			if(code >= 0){
				if(key = find_key(code))
					key->action.cmd();
				else
					if ((code >= 32) && (code < 128)){
						if (find_field(cx, cy)){
							enter_character(code);
							refresh();
						} else
							beep();
					} else
						eprint("%04o;", code);
			}
			if(r == 1) /* no data available on pty's */
				continue;
		}

	}

	cleanup();
	return 0;
}
