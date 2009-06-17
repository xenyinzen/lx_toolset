#include <ncurses.h>

#define WIDTH 15
#define HEIGHT 8 

int startx1 = 0;
int starty1 = 0;

int startx2 = 0;
int starty2 = 0;
int pressed = 0;

void print_menu(WINDOW *menu_win, int highlight);


int main()
{	int c, choice = 0;
	int ret = 0;
	WINDOW *menu_win;
	WINDOW *menu_win2;
	MEVENT event;

	/* Initialize curses */
	initscr();
	clear();
	noecho();
	cbreak();	//Line buffering disabled. pass on everything

	/* Try to put the window in the middle of screen */
	startx1 = 5;
	starty1 = 5;
	
	startx2 = 40;
	starty2 = 5;
	
	attron(A_REVERSE);
	mvprintw(24, 1, "Press 'n' or 'N' to record failure");
	attroff(A_REVERSE);
	refresh();

	/* Print the menu for the first time */
	menu_win = newwin(HEIGHT, WIDTH, starty1, startx1);
	menu_win2 = newwin(HEIGHT, WIDTH, starty2, startx2);
	print_menu(menu_win, 0);
	print_menu(menu_win2, 0);
	refresh();
	
	/* Get all the mouse events */
	mousemask(ALL_MOUSE_EVENTS, NULL);
	
	while (pressed != 3) {	
		c = wgetch(menu_win);
		switch (c) {	
		case KEY_MOUSE:
			if (getmouse(&event) == OK)	{	
				report_choice(event.x + 1, event.y + 1, &choice);
				if (choice == 1) {
					if (event.bstate & BUTTON1_PRESSED) {	
						pressed |= 1;
						refresh(); 
						print_menu(menu_win, 1);
					} 
				} else if (choice == 2) {
					if (event.bstate & BUTTON3_PRESSED) {
						pressed |= 2;
						refresh(); 
						print_menu(menu_win2, 1);
					} 
				}
			}
			break;
		case 'n':
		case 'N':
			ret = 1;
			goto end;
		default:
			break;
		}
	}
	
end:
	endwin();
	return ret;		

}


void print_menu(WINDOW *menu_win, int highlight)
{
	int i,j;
	box(menu_win, 0, 0);
	if (highlight) {
		wattron(menu_win, A_REVERSE); 
		for (j=0;j<HEIGHT;j++)
			for (i=0;i<WIDTH;i++)
				mvwprintw(menu_win, j, i, " ");
		wattroff(menu_win, A_REVERSE);
	} else {
		wattron(menu_win, A_NORMAL); 
		for (j=1;j<HEIGHT-1;j++)
			for (i=1;i<WIDTH-1;i++)
				mvwprintw(menu_win, j, i, " ");
		wattroff(menu_win, A_NORMAL);
	}

	wrefresh(menu_win);
}

void report_choice(int mouse_x, int mouse_y, int *p_choice)
{ 
	if ((mouse_x > startx1) && (mouse_x < (startx1 + WIDTH) ) && 
			(mouse_y > starty1) && (mouse_y < (starty1 + HEIGHT))) {
		*p_choice = 1;
	}
	if ((mouse_x > startx2) && (mouse_x < (startx2 + WIDTH) ) && 
			(mouse_y > starty2) && (mouse_y < (starty2 + HEIGHT))) {
		*p_choice = 2;
	}
}
