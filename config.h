/* valid curses attributes are listed below they can be ORed
 *
 * A_NORMAL        Normal display (no highlight)
 * A_STANDOUT      Best highlighting mode of the terminal.
 * A_UNDERLINE     Underlining
 * A_REVERSE       Reverse video
 * A_BLINK         Blinking
 * A_DIM           Half bright
 * A_BOLD          Extra bright or bold
 * A_PROTECT       Protected mode
 * A_INVIS         Invisible or blank mode
 * COLOR(fg,bg)    Color where fg and bg are one of:
 *
 *   COLOR_BLACK
 *   COLOR_RED
 *   COLOR_GREEN
 *   COLOR_YELLOW
 *   COLOR_BLUE
 *   COLOR_MAGENTA
 *   COLOR_CYAN
 *   COLOR_WHITE
 */
#define ATTR_NORMAL		A_NORMAL
#define ATTR_INPUT		A_UNDERLINE 
#define ATTR_INPUT_EMPTY	A_UNDERLINE
#define ATTR_INPUT_CHANGED	A_BOLD
#define ATTR_LABEL		A_NORMAL

Key keys[] = {
	{ CTRL('q'),	quit },
	{ KEY_DOWN,	cursor_down },
	{ KEY_UP,	cursor_up },
	{ KEY_LEFT,	cursor_left },
	{ KEY_RIGHT,	cursor_right },
	{ '\t',		next_field },
	{ '\n',		next_line },
	{ CTRL('l'),	redraw_screen },
};

