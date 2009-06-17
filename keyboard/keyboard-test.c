/*
 * Descriptions:	keyboard test draft
 * Author:			Tanggang, tangg@lemote.com
 * 					Huangwei, huangw@lemote.com
 * Date:			2008-05-15
 * License:			Public Domain
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <errno.h>
#include <ncurses.h>
#include <string.h>

#include "keyboard-test.h"


#define KEYNUMS 80
#define TIMELEFT 20

int tmp;	/* for debugging */
int fd;
int oldkbmode;
struct termios old;
int retval = 0;
int key_count = 0;


int main(int argc, char *argv[]) {

	struct termios new;
	unsigned char buf[18];	/* divisible by 3 */
	int i, n;

	fd = getfd(NULL);

	/* the program terminates when there is no input for TIMELEFT secs */
	signal(SIGALRM, watch_dog);
	/* if we receive a signal, we want to exit nicely, in
	 * order not to leave the keyboard in an unusable mode
	 */
	signal(SIGHUP, die);
	signal(SIGINT, die);
	signal(SIGQUIT, die);
	signal(SIGILL, die);
	signal(SIGTRAP, die);
	signal(SIGABRT, die);
	signal(SIGIOT, die);
	signal(SIGFPE, die);
	signal(SIGKILL, die);
	signal(SIGUSR1, die);
	signal(SIGSEGV, die);
	signal(SIGUSR2, die);
	signal(SIGPIPE, die);
	signal(SIGTERM, die);
	signal(SIGCHLD, die);
	signal(SIGCONT, die);
	signal(SIGSTOP, die);
	signal(SIGTSTP, die);
	signal(SIGTTIN, die);
	signal(SIGTTOU, die);

	/* get current term mode and print it out */
	get_mode();
	if (tcgetattr(fd, &old) == -1)
		perror("tcgetattr");
	if (tcgetattr(fd, &new) == -1)
		perror("tcgetattr");

	/* set new term attributes */
	new.c_lflag &= ~ (ICANON | ECHO | ISIG);
	new.c_iflag = 0;
	new.c_cc[VMIN] = sizeof(buf);
	new.c_cc[VTIME] = 1;	/* 0.1 sec intercharacter timeout */

	if (tcsetattr(fd, TCSAFLUSH, &new) == -1)
		perror("tcsetattr");
	if (ioctl(fd, KDSKBMODE, K_RAW)) {
		perror("KDSKBMODE");
		exit(1);
	}
	
	/* Open the curses library */
	initscr();
	/* this function is very important, don't remove it 
	 * there are one interesting thing between termios and curses, they can all control the term action, 
	 * but we don't know the in deep content when they interact. One thing I know is if we don't add the following sentence,
	 * we will be unstable when run. 
	 */
	raw();
	
	/* once using ncurses library, we could not use printf any more */
	mvprintw(5, 0, "Press key to see effect please.");
	mvprintw(6, 0, "Program will exit automatically after 10 seconds without pressing");
	refresh();
	
	/* print all key names and draw all frames on the screen */
	curs_set(0);  /* 0:hide cursor 1:display cursos */
	attron(A_BOLD);  /* highlight outline border */
	mvhline(7, 0, ACS_ULCORNER, 1);
	mvhline(7, 79, ACS_URCORNER, 1);
	mvhline(7, 1, ACS_HLINE, 78);
	mvhline(19, 1, ACS_HLINE, 78);
	mvhline(19, 0, ACS_LLCORNER, 1);
	mvhline(19, 79, ACS_LRCORNER, 1);
	for (i = 9; i < 18; i += 2){
		mvvline(i, 0, ACS_LTEE, 1);
		mvvline(i, 79, ACS_RTEE, 1);
	}
	for (i = 8; i <= 18; i += 2){
		mvvline(i, 0, ACS_VLINE, 1);
		mvvline(i, 79, ACS_VLINE, 1);
	}
	attroff(A_BOLD);  /* close highlight outline border */
	for (i = 9; i < 19; i = i + 2){
		mvhline(i, 1, ACS_HLINE, 78);
	}
 	for (i = 1; i <= 127; i++) {
	    if (keys[i].is_valid && !(keys[i].is_pressed)){
	        mvprintw(keys[i].row, keys[i].col, keys[i].keyname);

	    /* draw form line */
		if (keys[i].row == 8){
	 	    if (keys[i].col >= 1){
			attron(A_BOLD);
		        mvvline(keys[i].row - 1, keys[i].col + 3, ACS_TTEE, 1);
			attroff(A_BOLD);
		        mvvline(keys[i].row, keys[i].col + 3, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 3, ACS_BTEE, 1);
			mvvline(keys[i].row + 1, 36, ACS_PLUS, 1);
		    }
		}

		if (keys[i].row == 10){
		    if (keys[i].col == 74) continue;
		    if (keys[i].col == 1){
		        mvvline(keys[i].row - 1, keys[i].col + 5, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 5, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 5, ACS_BTEE, 1);
		    }
		    if (keys[i].col > 1 && keys[i].col < 74){
		        mvvline(keys[i].row - 1, keys[i].col + 3, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 3, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 3, ACS_BTEE, 1);
		    } 
		}

		if (keys[i].row == 12){
		    if (keys[i].col == 77) continue;
		    if (keys[i].col >= 6 && keys[i].col <= 62){
		        mvvline(keys[i].row - 1, keys[i].col + 3, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 3, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 3, ACS_BTEE, 1);
		    }
		    if (keys[i].col == 1){
		        mvvline(keys[i].row - 1, keys[i].col + 4, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 4, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 4, ACS_BTEE, 1);
		    }
		    if (keys[i].col > 66 && keys[i].col < 77){
		        mvvline(keys[i].row - 1, keys[i].col + 3, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 3, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 3, ACS_BTEE, 1);
		    }
		}

		if (keys[i].row == 14){
		    if (keys[i].col == 73) continue;
		    if (keys[i].col > 1 && keys[i].col <= 57){
		        mvvline(keys[i].row - 1, keys[i].col + 3, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 3, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 3, ACS_BTEE, 1);
		    }
		    if (keys[i].col == 1){
		        mvvline(keys[i].row - 1, keys[i].col + 5, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 5, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 5, ACS_BTEE, 1);
		    }
		    if (keys[i].col >= 62 && keys[i].col <= 67){
		        mvvline(keys[i].row - 1, keys[i].col + 3, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 3, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 3, ACS_BTEE, 1);
			mvvline(keys[i].row - 1, 65, ACS_PLUS, 1);
			mvvline(keys[i].row - 1, 70, ACS_PLUS, 1);
		    }
		}

		if (keys[i].row == 16){
		    if (keys[i].col == 74) continue;
		    if (keys[i].col >= 10 && keys[i].col <= 68){
		        mvvline(keys[i].row - 1, keys[i].col + 3, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 3, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 3, ACS_BTEE, 1);
			mvvline(keys[i].row - 1, 60, ACS_PLUS, 1);
			mvvline(keys[i].row - 1, 65, ACS_PLUS, 1);
		    }
		    if (keys[i].col == 1){
		        mvvline(keys[i].row - 1, keys[i].col + 6, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 6, ACS_VLINE, 1);
		        mvvline(keys[i].row + 1, keys[i].col + 6, ACS_BTEE, 1);
		    }
		}

		if (keys[i].row == 18){
		    mvvline(keys[i].row - 1, 11, ACS_TTEE, 1);
		    mvvline(keys[i].row, 11, ACS_VLINE, 1);
		    attron(A_BOLD);
		    mvvline(keys[i].row + 1, 11, ACS_BTEE, 1);
		    attroff(A_BOLD);
		    if (keys[i].col >= 1 && keys[i].col <= 18){
		        mvvline(keys[i].row - 1, keys[i].col + 5, ACS_TTEE, 1);
		        mvvline(keys[i].row, keys[i].col + 5, ACS_VLINE, 1);
			attron(A_BOLD);
		        mvvline(keys[i].row + 1, keys[i].col + 5, ACS_BTEE, 1);
			attroff(A_BOLD);
		    }
		    if (keys[i].col == 34){
		        mvvline(keys[i].row, keys[i].col + 15, ACS_VLINE, 1);
			attron(A_BOLD);
		        mvvline(keys[i].row + 1, keys[i].col + 15, ACS_BTEE, 1);
			attroff(A_BOLD);
			mvvline(keys[i].row - 1, keys[i].col + 15, ACS_PLUS, 1);
		    }
		}
	    }

	    if (e0_keys[i].is_valid && !(e0_keys[i].is_pressed)) {
		mvprintw(e0_keys[i].row, e0_keys[i].col, e0_keys[i].keyname);

		if (e0_keys[i].row == 8){
		    if (e0_keys[i].col == 76) continue;
		    if (e0_keys[i].col == 66){ 
			attron(A_BOLD);
		        mvvline(e0_keys[i].row - 1, e0_keys[i].col + 5, ACS_TTEE, 1);
			attroff(A_BOLD);
		        mvvline(e0_keys[i].row, e0_keys[i].col + 5, ACS_VLINE, 1);
		        mvvline(e0_keys[i].row + 1, e0_keys[i].col + 5, ACS_BTEE, 1);
		    }
		    if (e0_keys[i].col == 72){ 
			attron(A_BOLD);
			mvvline(e0_keys[i].row - 1, e0_keys[i].col + 3, ACS_TTEE, 1);
			attroff(A_BOLD);
			mvvline(e0_keys[i].row, e0_keys[i].col + 3, ACS_VLINE, 1);
			mvvline(e0_keys[i].row + 1, e0_keys[i].col + 3, ACS_BTEE, 1);
		    }
		}

		if (e0_keys[i].row == 16 && e0_keys[i].col == 68){
			mvvline(e0_keys[i].row - 1, e0_keys[i].col + 3, ACS_TTEE, 1);
			mvvline(e0_keys[i].row, e0_keys[i].col + 3, ACS_VLINE, 1);
			mvvline(e0_keys[i].row + 1, e0_keys[i].col + 3, ACS_BTEE, 1);
		}

		if (e0_keys[i].row == 18){
		    if (e0_keys[i].col == 74) continue;
		    if (e0_keys[i].col >= 12 && e0_keys[i].col < 74){
			    mvvline(e0_keys[i].row - 1, e0_keys[i].col + 5, ACS_TTEE, 1);
			    mvvline(e0_keys[i].row, e0_keys[i].col + 5, ACS_VLINE, 1);
			    attron(A_BOLD);
			    mvvline(e0_keys[i].row + 1, e0_keys[i].col + 5, ACS_BTEE, 1);
			    attroff(A_BOLD);
			    mvvline(keys[i].row - 1, 55, ACS_PLUS, 1);
		    }
		}
	    }
	}
	refresh();

	while (1) {
		alarm(TIMELEFT);
		n = read(fd, buf, sizeof(buf));

		/* retval means how many keys have been pressed */
		retval = count_keys( buf, n );

		if (retval >= KEYNUMS) {
			mvprintw( 20, 0, "Keyboard test PASS!\n" );
			refresh();
			endwin();
			clean_up();
			return 0;
		}
	}
}


static int is_a_console(int fd) 
{
	char arg;

	arg = 0;
	return (ioctl(fd, KDGKBTYPE, &arg) == 0
		&& ((arg == KB_101) || (arg == KB_84)));
}

static int open_a_console(const char *fnam) 
{
	int fd;

	/*
	 * For ioctl purposes we only need some fd and permissions
	 * do not matter. But setfont:activatemap() does a write.
	 */
	fd = open(fnam, O_RDWR);
	if (fd < 0 && errno == EACCES)
		fd = open(fnam, O_WRONLY);
	if (fd < 0 && errno == EACCES)
		fd = open(fnam, O_RDONLY);
	if (fd < 0)
		return -1;
	if (!is_a_console(fd)) {
		close(fd);
		return -1;
	}
	return fd;
}


/*
 * getfd.c
 *
 * Get an fd for use with kbd/console ioctls.
 * We try several things because opening /dev/console will fail
 * if someone else used X (which does a chown on /dev/console).
 */
int getfd(const char *fnam) 
{
	int fd;

	if (fnam) {
		fd = open_a_console(fnam);
		if (fd >= 0)
			return fd;
		fprintf(stderr,
			"Couldnt open %s\n", fnam);
		exit(1);
	}

	fd = open_a_console("/dev/tty");
	if (fd >= 0)
		return fd;

	fd = open_a_console("/dev/tty0");
	if (fd >= 0)
		return fd;

	fd = open_a_console("/dev/vc/0");
	if (fd >= 0)
		return fd;

	fd = open_a_console("/dev/console");
	if (fd >= 0)
		return fd;

	for (fd = 0; fd < 3; fd++)
		if (is_a_console(fd))
			return fd;

	fprintf(stderr,
		"Couldnt get a file descriptor referring to the console\n");
	exit(1);		/* total failure */
}

/*
 * version 0.81 of showkey would restore kbmode unconditially to XLATE,
 * thus making the console unusable when it was called under X.
 */
static void get_mode(void) 
{
    char *m;

	if (ioctl(fd, KDGKBMODE, &oldkbmode)) {
		perror("KDGKBMODE");
		exit(1);
	}
	switch(oldkbmode) {
	  case K_RAW:
	    m = "RAW"; break;
	  case K_XLATE:
	    m = "XLATE"; break;
	  case K_MEDIUMRAW:
	    m = "MEDIUMRAW"; break;
	  case K_UNICODE:
	    m = "UNICODE"; break;
	  default:
	    m = "?UNKNOWN?"; break;
	}
	mvprintw(0, 0, "kb mode was");
	if (oldkbmode != K_XLATE) {
	    mvprintw(0, 0, "[ if you are trying this under X, it might not work since the X server is also reading /dev/console ]");
	}
}

static void clean_up(void) 
{
	if (ioctl(fd, KDSKBMODE, oldkbmode)) {
		perror("KDSKBMODE");
		exit(1);
	}
	if (tcsetattr(fd, TCSANOW, &old) == -1)
		perror("tcsetattr");
	close(fd);
}

static void die(int x) 
{
	//mvprintw( 0, 0, "caught signal, cleaning up..."	);
	endwin();
	clean_up();
	exit(-1);
}

static void watch_dog(int x) 
{
	//mvprintw( 18, 0, "Keyboard test FAILED\n" );
	endwin();
	clean_up();
	exit(1);
}

int count_keys( unsigned char *buf, int n )
{
	int i,j,m;
	unsigned int ch = 0;
	unsigned char buf1[18] = {0};

	for (i = 0; i < n; i++) {
		ch = buf[i];

		/* "PauseBreak" key */
		if (ch == 0x1d && buf[i+1] == 0x45){
			if (!(e0_keys[ch].is_pressed) && e0_keys[ch].is_valid){
				e0_keys[ch].is_pressed = 1;
				m = strlen(e0_keys[ch].keyname);
				for (j = 0; j < m; j++)
					buf1[j] = ' ';
				m = '\0';
				mvprintw( e0_keys[ch].row, e0_keys[ch].col, buf1);
				refresh();
				key_count++;
			}
		}		
		/* Extend key */
		else if ( buf[0] == 0xe0){
			if (!(e0_keys[ch].is_pressed) && e0_keys[ch].is_valid){
				e0_keys[ch].is_pressed = 1;
				m = strlen(e0_keys[ch].keyname);
				for (j = 0; j < m; j++)
					buf1[j] = ' ';
				m = '\0';
				mvprintw( e0_keys[ch].row, e0_keys[ch].col, buf1);
				refresh();
				key_count++;
			}
		}		
		/* standard key */
		else if (ch <= 0x7F && buf[0] != 0xe0) {
			if (!(keys[ch].is_pressed) && keys[ch].is_valid){
				keys[ch].is_pressed = 1;
				m = strlen(keys[ch].keyname);
				for (j = 0; j < m; j++)
					buf1[j] = ' ';
				m = '\0';
				mvprintw( keys[ch].row, keys[ch].col, buf1);
				refresh();
				key_count++;
			}
		}
		
		if (key_count > KEYNUMS)
			return key_count;
	}/* endfor */
	
	/* return how many keys have been pressed */
	return key_count;
} 

