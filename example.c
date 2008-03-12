/*
 * forms example
 * based on Chapter 18 from NCURSES Programming HOWTO
 */

#include <form.h>

int main()
{
	FIELD *field[3];
	FORM  *my_form;
	int insert_mode;
	int ch;
	
	/* Initialize curses */
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	/* Initialize the fields */
	field[0] = new_field(1, 10, 4, 18, 0, 0);
	field[1] = new_field(1, 10, 6, 18, 0, 0);
	field[2] = NULL;

	/* Set field options */
	set_field_back(field[0], A_UNDERLINE); 	/* Print a line for the option 	*/
	field_opts_off(field[0], O_AUTOSKIP);  	/* Don't go to next field when this */
						/* Field is filled up 		*/
	set_field_back(field[1], A_UNDERLINE); 
	field_opts_off(field[1], O_AUTOSKIP);

	/* Create the form and post it */
	my_form = new_form(field);
	post_form(my_form);
	refresh();
	
	mvprintw(4, 10, "Value 1:");
	mvprintw(6, 10, "Value 2:");
	refresh();

	/* set overwrite mode */
	form_driver(my_form, REQ_OVL_MODE);
	insert_mode = 0;

	/* Loop through to get user requests */
	while((ch = getch()) != KEY_F(3)) {
		switch(ch) {	
		case '\t':
			form_driver(my_form, REQ_NEXT_FIELD);
			form_driver(my_form, REQ_BEG_LINE);
			break;
		case KEY_BTAB:
			form_driver(my_form, REQ_PREV_FIELD);
			form_driver(my_form, REQ_BEG_LINE);
			break;
		case KEY_LEFT:
			form_driver(my_form, REQ_LEFT_CHAR);
			break;
		case KEY_RIGHT:
			form_driver(my_form, REQ_RIGHT_CHAR);
			break;
		case KEY_END:
			form_driver(my_form, REQ_CLR_EOL);
			break;
		case KEY_IC:
			insert_mode = 1 - insert_mode;
			if (insert_mode)
				form_driver(my_form, REQ_INS_MODE);
			else
				form_driver(my_form, REQ_OVL_MODE);
		default:
			/* If this is a normal character, it gets */
			/* Printed				  */	
			form_driver(my_form, ch);
//			printf("%04o", ch);
			break;
		}
	}
	
	/* Un post form and free the memory */
	unpost_form(my_form);
	free_form(my_form);
	free_field(field[0]);
	free_field(field[1]); 

	endwin();
	return 0;
}
