#ifndef __vidd_h__
#define __vidd_h__

#include <ddcDef.h>
#include <ddcBits.h>
#include <ddcString.h>
#include <ddcColors.h>
#include <ddcKeyboard.h>
#include <ddcPrint.h>
#include <ddcMem.h>

#include "file.h"
#include "line.h"

#define IDX(x, y, w) (y*w+x)
#define IS_LETTER(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))

enum
{
	VIDD_MODE_NORMAL=0,
	VIDD_MODE_INSERT,
	VIDD_MODE_REPLACE,
};

struct cele;
struct client;

void vidd_copy_line(struct client* c);
void vidd_paste_line(struct client* c);
void vidd_goto_end(struct client* c);
void vidd_goto_start(struct client* c);
void vidd_delete_line(struct client* c);
void vidd_delete_commands(struct client* c);
void vidd_insert_line_down(struct client* c);
void vidd_skip_word(struct client* c);
void vidd_insert_line_up(struct client* c);
void vidd_right_insert(struct client* c);
void vidd_exit(struct client* c);
void vidd_adjust_x(struct client* c);
void vidd_move_force_right(struct client* c);
void vidd_move_up(struct client* c);
void vidd_move_down(struct client* c);
void vidd_move_left(struct client* c);
void vidd_move_right(struct client* c);
void vidd_pos_up(struct client* c);
void vidd_pos_down(struct client* c);
void vidd_pos_left(struct client* c);
void vidd_pos_right(struct client* c);
void vidd_delete(struct client* c);
void vidd_backspace(struct client* c);
void vidd_enter_normal_mode(struct client* c);
void vidd_enter_insert_mode(struct client* c);
void vidd_enter_replace_mode(struct client* c);
void vidd_cursor_refresh(struct client* c);
void vidd_print_buffer(struct client c);
void vidd_print(struct client c);
void vidd_redraw(struct client c);
int vidd_main(void);

struct client cmaster;


struct cursor
{
	int x;
	struct line* y;
};

struct buffer
{
	char* data;
	long len;
};

struct client
{
	int width, height;
	int x, y;
	int mode;
	int spos;
	struct cursor cur;
	struct buffer buf;
};

#include "../config.h"

void vidd_goto_end(struct client* c)
{
	while (c->cur.x+1 < c->cur.y->len)
		vidd_move_right(c);
}
void vidd_goto_start(struct client* c)
{
	cursor_return();
	c->cur.x = 0;
}
void vidd_delete_line(struct client* c)
{
	struct line* n = c->cur.y->next;
	if (c->cur.y->prev)
	{
		c->cur.y->prev->next = c->cur.y->next;
	}
	if (c->cur.y->next)
	{
		c->cur.y->next->prev = c->cur.y->prev;
	}
	if (c->cur.y->next)
	{
		c->cur.y = c->cur.y->next;
		ddPrintf("\x1b[J");
		vidd_redraw(*c);
	}
	else
	{
		vidd_move_up(c);
		ddPrintf("\x1b[J");
		vidd_redraw(*c);
	}
	vidd_adjust_x(c);
}
void vidd_delete_commands(struct client* c)
{
	char key = ddKey_getch_noesc();
	switch (key)
	{
		case 'd':
		{
			vidd_delete_line(c);
		} break;
		case 'k':
		{
			vidd_move_up(c);
			vidd_delete_line(c);
			vidd_delete_line(c);
		} break;
		case 'j':
		{
			vidd_delete_line(c);
			vidd_delete_line(c);
		} break;
	}
}
void vidd_skip_word(struct client* c)
{
	while (IS_LETTER(c->cur.y->text[c->cur.x]))
		vidd_move_right(c);
}
void vidd_insert_line_down(struct client* c)
{
	c->cur.x = c->cur.y->len;
	line_split(c->cur.y, c->cur.x);
	vidd_redraw(*c);
	vidd_enter_insert_mode(c);
	vidd_move_down(c);
	cursor_return();
	c->cur.x = 0;
}
void vidd_insert_line_up(struct client* c)
{
	cursor_return();
	c->cur.x = 0;
	line_split(cmaster.cur.y, cmaster.cur.x);
	vidd_redraw(*c);
	vidd_enter_insert_mode(c);
}
void vidd_right_insert(struct client* c)
{
	vidd_enter_insert_mode(c);
	vidd_move_force_right(c);
}
void vidd_exit(struct client* c)
{
	exit(0);
}

void vidd_copy_line(struct client* c)
{
	if (!c->buf.data) free(c->buf.data);
	c->buf.data = malloc(c->cur.y->len);
	c->buf.len = c->cur.y->len;
	mem_copy(c->buf.data, c->cur.y->text, c->buf.len);
}
void vidd_paste_line(struct client* c)
{
	vidd_insert_line_down(c);
	mem_copy(c->cur.y->text, c->buf.data, c->buf.len);
	c->cur.y->len = c->buf.len;
	vidd_redraw(*c);
	vidd_enter_normal_mode(c);
}

void vidd_adjust_x(struct client* c)
{
	if (c->cur.x == 0) return;
	if (c->cur.y->len == 0)
	{
		c->cur.x = 0;
		cursor_return();
		return;
	}
	if (c->cur.x >= c->cur.y->len) c->cur.x = c->cur.y->len-1;
	cursor_return();
	cursor_move(c->cur.x, 1);
	cursor_up();
}

void vidd_move_force_right(struct client* c)
{
	c->cur.x++;
	cursor_right();
}
void vidd_move_up(struct client* c)
{
	if (c->cur.y->prev)
	{
		if (c->cur.y->num == c->spos)
		{
			c->spos--;
			vidd_redraw(*c);
		}
		c->cur.y = c->cur.y->prev;
		cursor_up();
		vidd_adjust_x(c);
	}
}
void vidd_move_down(struct client* c)
{
	if (c->cur.y->next)
	{
		if (c->cur.y->num == c->spos+c->height-2)
		{
			c->spos++;
			vidd_redraw(*c);
		}
		c->cur.y = c->cur.y->next;
		cursor_down();
		vidd_adjust_x(c);
	}
}
void vidd_move_right(struct client* c)
{
	if (c->cur.x+1 < c->cur.y->len)
	{
		c->cur.x++;
		cursor_right();
	}
}
void vidd_move_left(struct client* c)
{
	if (c->cur.x > 0)
	{
		c->cur.x--;
		cursor_left();
	}
}

void vidd_delete(struct client* c)
{
	if (c->cur.y->len == 0) return;
	vidd_adjust_x(c);
	line_delete(c->cur.y, c->cur.x);
	line_redraw(c->cur.y);
	vidd_adjust_x(c);
}
void vidd_backspace(struct client* c)
{
	if (c->cur.x == 0) return;
	line_delete(c->cur.y, c->cur.x-1);
	line_redraw(c->cur.y);
	vidd_move_left(c);
}

void vidd_enter_normal_mode(struct client* c)
	{ c->mode = VIDD_MODE_NORMAL; };
void vidd_enter_insert_mode(struct client* c)
	{ c->mode = VIDD_MODE_INSERT; };
void vidd_enter_replace_mode(struct client* c)
	{ c->mode = VIDD_MODE_REPLACE; };

void vidd_redraw(struct client c)
{
	cursor_save();
	vidd_print(c);
	cursor_restore();
}
void vidd_print(struct client c)
{
	cursor_clear();
	cursor_home();
	struct line* l = line_get_line(c.cur.y, c.spos);
	for (int i = 0; i < c.height-1 && l; i++)
	{
		ddPrint(l->text, l->len);
		ddPrint_nl();
		l = l->next;
	}
}

int vidd_main(void)
{
	for (;;)
	{
		char key = ddKey_getch_noesc();
		switch (cmaster.mode)
		{
			case VIDD_MODE_NORMAL:
			{
				mode_normal_functions[key](&cmaster);
			} break;
			case VIDD_MODE_REPLACE:
			{
				if (key == 27)
				{
					vidd_move_left(&cmaster);
					vidd_enter_normal_mode(&cmaster);
					break;
				}
				line_replace(cmaster.cur.y, cmaster.cur.x, key);
				ddPrint_char(key);
			} break;
			case VIDD_MODE_INSERT:
			{
				if (key == 27)
				{
					vidd_move_left(&cmaster);
					vidd_enter_normal_mode(&cmaster);
					break;
				}
				else if (key == '\n')
				{
					line_split(cmaster.cur.y, cmaster.cur.x);
					vidd_redraw(cmaster);
					vidd_move_down(&cmaster);
					cursor_return();
					cmaster.cur.x = 0;
					break;
				}
				else if (key == DDK_BACKSPACE)
				{
					vidd_backspace(&cmaster);
					break;
				}
				line_insert(cmaster.cur.y, cmaster.cur.x, key);
				line_redraw(cmaster.cur.y);
				vidd_move_force_right(&cmaster);
			} break;
		}
	}
}

int main(int argc, char** argv)
{
	ddString data = read_file(argv[1]);
	data.length++;

	cmaster.width = cursor_get_width();
	cmaster.height = cursor_get_height();
	cmaster.x = 0;
	cmaster.y = 0;

	cmaster.mode = VIDD_MODE_NORMAL;

	cmaster.buf.data = 0;

	cmaster.cur.x = 0;
	cmaster.cur.y = make_line(0, 0, 0, 100);
	cmaster.spos = 0;

	int newline_count = 0;
	for (int i = 0; i < data.length; i++)
		if (data.cstr[i] == '\n') 
			newline_count++;

	int pos = 0;
	for (int i = 0; i < data.length; i++)
	{
		if (data.cstr[i] == '\n')
		{
			cmaster.cur.y->next = make_line(0, cmaster.cur.y, cmaster.cur.y->num+1, 100);
			cmaster.cur.y = cmaster.cur.y->next;
			pos++;
			continue;
		}
		line_append(cmaster.cur.y, data.cstr[i]);
	}
	cmaster.cur.y->next = (0);

	vidd_print(cmaster);
	cursor_home();

	cmaster.cur.y = line_get_line(cmaster.cur.y, cmaster.spos);

	vidd_main();

	

	return 0;
}

#endif
