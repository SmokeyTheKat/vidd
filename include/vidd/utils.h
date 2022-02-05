#ifndef __VIDD_UTILS_H__
#define __VIDD_UTILS_H__

#include <stdint.h>
#include <stdbool.h>

#define IS_CHARACTER(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_'))
#define IS_NUMBER(c) (c >= '0' && c <= '9')
#define IS_SPECIAL(c) ((c >= ' ' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~'))
#define IS_PRINTABLE(c) (c >= ' ' && c <= '~' && c != 127)
#define MAX(a, b) ((a >= b) ? (a) : (b))
#define MIN(a, b) ((a <= b) ? (a) : (b))
#define SWAP(a, b) { typeof(a) tmp = a; a = b; b = tmp; }
#define ABS(v) (((v) >= 0) ? ((v)) : (-1*(v)))

#define CURSOR_HIDE "\x1b[?25l"
#define CURSOR_SHOW "\x1b[?25h"
#define CURSOR_TO(x, y) "\x1b[" y ";" x "H"
#define CURSOR_MOVE(x, y) "\x1b[" x "C" "\x1b[" y "B"
#define CURSOR_UP(v) "\x1b[" v "A"
#define CURSOR_DOWN(v) "\x1b[" v "B"
#define CURSOR_RIGHT(v) "\x1b[" v "C"
#define CURSOR_LEFT(v) "\x1b[" v "D"

#define PATH_MAX 4096

enum
{
	KEY_UP=438,
	KEY_DOWN=439,
	KEY_RIGHT=440,
	KEY_LEFT=441,
	KEY_DELETE=295,
	KEY_BACKSPACE=127,
	KEY_CTRL_BACKSLASH=28,
	KEY_RETURN=13,
	KEY_SHIFT_RETURN=438,
	KEY_ESCAPE=27,
	KEY_CTRL_BACKSPACE=8,
	KEY_TAB='\t',
};

#define VIDD_LINE_SELECTION_FULL_LOOP(c) \
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
#define VIDD_LINE_SELECTION_LOOP(c) \
	{ \
		intmax_t y0 = client->selection.y0; \
		intmax_t y1 = client->selection.y1; \
		if (y0 > y1) SWAP(y0, y1); \
		y0 = MAX(client->view.y, y0); \
		y1 = MIN(client->view.y + client->view.height-1, y1); \
		line = line_get_line(client->cursor.y, y0+1); \
		intmax_t lim = y1; \
		for (intmax_t i = y0; i <= lim && line; i++) \
		{ \
			c \
		} \
	}

#define VIDD_SELECTION_FULL_LOOP_CRSMEJ(c, r, s, m, e, J) \
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
			/*if (line->number-1 < client->view.y && \
				line->number >= client->view.y + client->view.height) \
			{ \
				line = line->next; \
				continue; \
			}*/ \
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
		if (y0 < client->view.y) \
		{ \
			y0 = client->view.y; \
			x0 = 0; \
		} \
		if (y1 >= client->view.y + client->view.height) \
		{ \
			y1 = client->view.y + client->view.height - 1; \
			x1 = 696969696969696969; \
		} \
		line = line_get_line(client->cursor.y, y0+1); \
		struct line* first_line = line; \
		struct line* last_line = 0; \
		intmax_t lim = y1 + dy; \
		for (intmax_t i = y0; i != lim && line; i += dy) \
		{ \
			/*if (line->number-1 < client->view.y && \
				line->number >= client->view.y + client->view.height) \
			{ \
				line = line->next; \
				continue; \
			}*/ \
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

char cstring_is_n_number(char* cstr, intmax_t n);
char* strlstr(char* src, char* find, intmax_t src_length);
int visable_strlen(char* str);
bool string_includes_list(char* str, char* list);
void set_terminal_title(char* title);
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

static const char CSET_LETTERS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-";
static const char CSET_NUMBERS[] = "0123456789";
static const char CSET_CHARACTERS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";

#endif