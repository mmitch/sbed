/*
 * sbed - screen based editor
 * (c) 2008 Christian Garbs <mitch@cgarbs.de>
 *
 * Basic design is highly inspired by the dvtm and reuses some code of it
 * which is mostly
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
#include <form.h>
#include <stdio.h>
#include <signal.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>

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

#define countof(arr) (sizeof (arr) / sizeof((arr)[0]))
#define sstrlen(str) (sizeof (str) - 1)
#define max(x, y) ((x) > (y) ? (x) : (y))

#ifdef NDEBUG
 #define debug(format, args...)
#else
 #define debug eprint
#endif

/* commands for use by keybindings */
void quit();
void redraw_screen();

/* internal functions */
void draw_all();

unsigned int bh = 1, by, waw, wah, wax, way;

#include "config.h"

const char *shell;
bool need_screen_resize = true;
int width, height;
bool running = true;

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
	getmaxyx(stdscr, height, width);
	resize_screen();
	signal(SIGWINCH, sigwinch_handler);
	signal(SIGCHLD, sigchld_handler);
	signal(SIGTERM, sigterm_handler);
}

void
cleanup(){
	endwin();
}

void draw_all(){
	wrefresh(curscr);
}

void redraw_screen(){
	wrefresh(curscr);
	draw_all();
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

int
main(int argc, char *argv[]){
	setup();
	wrefresh(curscr);
	
	
	/* test entries */
	FIELD *field1, *field2, *field3, *f;
	FIELD *fields[] = {field1, field2, field3, NULL};
	FORM  *form;
	
	field1 = new_field(1, 15, 2,  4, 0, 0);
	field2 = new_field(1, 20, 5,  4, 0, 0);
	field3 = new_field(1, 20, 5, 30, 0, 0);

	form = new_form(fields);
	
	post_form(form);
	
	wrefresh(curscr);

	unpost_form(form);
	
	free_form(form);
	
	for (f = fields[0]; f != NULL; f++){
		free_field(f);
	}
	
	cleanup();
	return 0;
}

/*
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
						if (find_field(cursor)){
							enter_character(code);
							refresh();
						} else
							beep();
					} else
						eprint("%04o;", code);
			}
			if(r == 1) /* no data available on pty's * /
				continue;
		}

	}

	cleanup();
	return 0;
}
*/
