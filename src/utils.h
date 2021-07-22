#ifndef __VIDD_UTILS_H__
#define __VIDD_UTILS_H__


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define ERROR_PRINT(v) {screen_clear();v;exit(0);}

#define IS_CHARACTER(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_'))
#define IS_NUMBER(c) (c >= '0' && c <= '9')
#define IS_SPECIAL(c) ((c >= ' ' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~'))
#define IS_PRINTABLE(c) (c >= ' ' && c <= '~' && c != 127)
#define MAX(a, b) ((a >= b) ? (a) : (b))
#define MIN(a, b) ((a <= b) ? (a) : (b))
#define SWAP(a, b) { typeof(a) tmp = a; a = b; b = tmp; }

#define FRGB(r, g, b) "\x1b[38;2;" r ";" g ";" b "m"
#define BRGB(r, g, b) "\x1b[48;2;" r ";" g ";" b "m"

#define NOSTYLE "\x1b[0m"
#define STYLE_UNDERLINE "\x1b[7m"
#define STYLE_ITALIC "\x1b[3m"

#define CURSOR_TO(x, y) "\x1b[" y ";" x "H"
#define CURSOR_UP(v) "\x1b[" v "A"
#define CURSOR_DOWN(v) "\x1b[" v "B"
#define CURSOR_RIGHT(v) "\x1b[" v "C"
#define CURSOR_LEFT(v) "\x1b[" v "D"

enum
{
	KEY_UP=438,
	KEY_DOWN=439,
	KEY_RIGHT=440,
	KEY_LEFT=441,
	KEY_DELETE=295,
	KEY_BACKSPACE=127,
	KEY_ESCAPE=27,
	KEY_CTRL_BACKSPACE=8,
};

#define VIDD_LINE_SELECTION_LOOP(c) \
	{ \
		intmax_t y0 = client->selection.y0; \
		intmax_t y1 = client->selection.y1; \
		if (y0 > y1) SWAP(y0, y1); \
		line = line_get_line(client->cursor.y, y0+1); \
		intmax_t lim = y1; \
		for (intmax_t i = y0; i <= lim && line; i++) \
		{ \
			c \
		} \
	}
#define VIDD_SELECTION_LOOP_CRSMEJ(c, r, s, m, e, J) \
	{ \
		intmax_t y0 = client->selection.y0; \
		intmax_t y1 = client->selection.y1; \
		intmax_t x0 = client->selection.x0; \
		intmax_t x1 = client->selection.x1; \
		int dy = 1; \
		if (y0 > y1) \
		{ \
			SWAP(y0, y1); \
			SWAP(x0, x1); \
		} \
		else if (x0 > x1 && y0 == y1) \
		{ \
			SWAP(x0, x1); \
		} \
		line = line_get_line(client->cursor.y, y0+1); \
		struct line* first_line = line; \
		struct line* last_line = 0; \
		intmax_t lim = y1 + dy; \
		for (intmax_t i = y0; i != lim && line; i += dy) \
		{ \
			c \
			if (i == y0) \
			{ \
				if (y0 == y1) r \
				else s \
			} \
			else if (i == y1) \
			{ \
				last_line = line; \
				e \
				J \
			} \
			else \
			{ \
				m \
			} \
			line = line->next; \
		} \
		if (line) line = line->prev; \
		(void)first_line; \
		(void)last_line; \
	}
#define VIDD_SELECTION_LOOP(c) { \
	VIDD_SELECTION_LOOP_CRSEMJ({}, {}, { \
				for (int j = x0; j < line_end; j++) \
				{ \
					chr = line->buffer.data[j]; \
					c \
				} \
				if (y0 != y1) \
				{ \
					chr = '\n'; \
					c \
				} \
		}, {\
				for (int j = 0; j < line_end; j++) \
				{ \
					chr = line->buffer.data[j]; \
					c \
				} \
		}, {\
				for (int j = 0; j < line_end; j++) \
				{ \
					chr = line->buffer.data[j]; \
					c \
				} \
				chr = '\n'; \
				c \
		}, {}) \
	}

void getch_exit(void);
void getch_init(void);
char cstring_is_n_number(char* cstr, intmax_t n);
intmax_t number_get_length(intmax_t num);
void screen_get_size(intmax_t * width, intmax_t* height);
void screen_save(void);
void screen_restore(void);
void screen_clear(void);
void cursor_move(int x, int y);
void cursor_move_to(int x, int y);
void cursor_return(void);
void cursor_erase_line(void);
void cursor_save(void);
void cursor_restore(void);
void cursor_home(void);

const char CSET_LETTERS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const char CSET_NUMBERS[] = "0123456789";
const char CSET_CHARACTERS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";

struct termios oldt;

void getch_exit(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
}

void getch_init(void)
{
	tcgetattr(STDIN_FILENO, &oldt);
	struct termios newt = oldt;
	newt.c_iflag &= ~(BRKINT | INPCK | ISTRIP | IXON);
	newt.c_oflag &= ~(OPOST);
	newt.c_cflag |= (CS8);
	newt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	newt.c_cc[VMIN] = 1;
	newt.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
}

char cstring_is_n_number(char* cstr, intmax_t n)
{
	intmax_t i;
	for (i = 0; cstr[i] && i < n; i++)
		if (!IS_NUMBER(cstr[i]))
			return false;
	return i == n;
}

intmax_t number_get_length(intmax_t num)
{
	switch (num)
	{
		case 0 ... 9: return 1;
		case 10 ... 99: return 2;
		case 100 ... 999: return 3;
		case 1000 ... 9999: return 4;
		case 10000 ... 99999: return 5;
		case 100000 ... 999999: return 6;
		case 1000000 ... 9999999: return 7;
		case 10000000 ... 99999999: return 8;
		case 100000000 ... 999999999: return 9;
		case 1000000000 ... 9999999999: return 10;
		case 10000000000 ... 99999999999: return 11;
		case 100000000000 ... 999999999999: return 12;
		case 1000000000000 ... 9999999999999: return 13;
		case 10000000000000 ... 99999999999999: return 14;
		case 100000000000000 ... 999999999999999: return 15;
		case 1000000000000000 ... 9999999999999999: return 16;
		case 10000000000000000 ... 99999999999999999: return 17;
		case 100000000000000000 ... 999999999999999999: return 18;
	}
	return 0;
}

void screen_get_size(intmax_t * width, intmax_t* height)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	*width = w.ws_col;
	*height = w.ws_row;
}

void screen_save(void)
{
	system("tput smcup");
}
void screen_restore(void)
{
	system("tput rmcup");
}


void screen_clear(void)
{
	printf("\x1b[2J\x1b[1;1H");
}

void cursor_move(int x, int y)
{
	if (y != 0)
	{
		if (y < 0) printf("\x1b[%dA", -1*y);
		else printf("\x1b[%dB", y);
	}
	if (x != 0)
	{
		if (x < 0) printf("\x1b[%dD", -1*x);
		else printf("\x1b[%dC", x);
	}
}
void cursor_move_to(int x, int y)
{
	printf("\x1b[%d;%dH", y+1, x+1);
}
void cursor_return(void)
{
	printf("\r");
}
void cursor_erase_line(void)
{
	printf("\x1b[2K");
}
void cursor_save(void)
{
	printf("\x1b[s");
}
void cursor_restore(void)
{
	printf("\x1b[u");
}
void cursor_home(void)
{
	cursor_move_to(0, 0);
}


#endif
