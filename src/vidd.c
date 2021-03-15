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
#define IS_LETTER(c) ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
#define CLAMP(v, m) ((v > m) ? m : v)
#define ABS(x) (((x) >= 0) ? (x) : -(x))

enum
{
	VIDD_MODE_NORMAL=0,
	VIDD_MODE_INSERT,
	VIDD_MODE_REPLACE,
	VIDD_MODE_SELECT,
	VIDD_MODE_LINE_SELECT,
};

struct style;
struct status;
struct buffer;
struct cursor;
struct selecter;
struct client;
struct command;

void vidd_void(struct client* c);

int vidd_main(void);
void vidd_read_command(struct client* c);
void vidd_handel_key(char key);

void vidd_write(struct client* c, char* dat);
void vidd_exit(struct client* c, char* dat);
void vidd_force_exit(struct client* c, char* dat);

void vidd_draw_status_start(struct client* c);
void vidd_draw_status(struct client* c);

void vidd_start_macro(struct client* c);
void vidd_run_macro(struct client* c);

void vidd_enter_normal_mode(struct client* c);
void vidd_enter_insert_mode(struct client* c);
void vidd_enter_select_mode(struct client* c);
void vidd_enter_line_select_mode(struct client* c);
void vidd_enter_replace_mode(struct client* c);
void vidd_right_insert(struct client* c);

void vidd_move_up(struct client* c);
void vidd_move_down(struct client* c);
void vidd_move_left(struct client* c);
void vidd_move_right(struct client* c);
void vidd_move_force_right(struct client* c);

void vidd_pos_up(struct client* c);
void vidd_pos_down(struct client* c);
void vidd_pos_left(struct client* c);

void vidd_goto_top(struct client* c);
void vidd_goto_bottom(struct client* c);
void vidd_goto_end(struct client* c);
void vidd_goto_start(struct client* c);
void vidd_skip_word(struct client* c);
void vidd_skip_word_back(struct client* c);

void vidd_indent(struct client* c);
void vidd_deindent(struct client* c);

void vidd_insert_at_start(struct client* c);
void vidd_insert_at_end(struct client* c);
void vidd_insert_line_down(struct client* c);
void vidd_insert_line_up(struct client* c);
void vidd_delete(struct client* c);
void vidd_delete_line(struct client* c);
void vidd_delete_commands(struct client* c);
void vidd_backspace(struct client* c);

void vidd_start_select(struct client* c);
void vidd_stop_select(struct client* c);
void vidd_copy(struct client* c);
void vidd_copy_line(struct client* c);
void vidd_paste_line(struct client* c);

void vidd_adjust_x(struct client* c);

void vidd_cursor_refresh(struct client* c);

void vidd_center(struct client* c);

void vidd_print_buffer(struct client c);
void vidd_print(struct client c);
void vidd_redraw(struct client c);
void vidd_redraw_line(struct client c);

struct client cmaster;

struct keyword
{
	char* word;
	long len;
	char* style;
	bool embedded;
};

struct style
{
	char* extension;
	struct keyword keywords[100];
};

struct status
{
	bool changed;
};

struct command
{
	char* com;
	void(*function)(struct client*,char*);
};

struct selecter
{
	struct line* y1;
	long x1;
	struct line* y2;
	long x2;
};

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

struct macro
{
	char* data;
	long len, cap;
	bool recording;
};

struct client
{
	char* filename;
	int width, height;
	int x, y;
	int mode;
	int spos;
	int vpos;
	struct cursor cur;
	struct buffer buf;
	struct macro mac;
	struct selecter sel;
	struct status sts;
	struct style sty;
};

#include "../config.h"

void vidd_void(struct client* c)
{
	return;
}

void vidd_show_message(struct client* c, char* msg)
{
	cursor_save();
	vidd_draw_status_start(c);
	ddPrintf(" "CRED"%s"CWHITE, msg);
	sleep(1);
	cursor_restore();
	vidd_draw_status(c);
}

void vidd_force_exit(struct client* c, char* dat)
{
	cursor_clear();
	cursor_home();
	exit(0);
}
void vidd_exit(struct client* c, char* dat)
{
	if (c->sts.changed)
	{
		vidd_show_message(c, "UNSAVED CHANGED");
	}
	else
	{
		cursor_clear();
		cursor_home();
		exit(0);
	}
}

void vidd_write(struct client* c, char* dat)
{
	ddString data = make_ddString_capacity("", line_get_last(c->cur.y)->num*100);
	struct line* l = line_get_first(c->cur.y);
	while (l)
	{
		for (int i = 0; i < l->len; i++)
		{
				if (*(int*)&l->text[i] == *(int*)"	")
				{
					ddString_push_char_back(&data, '	');
					i += 3;
				}
				else ddString_push_char_back(&data, l->text[i]);
		}
		ddString_push_char_back(&data, '\n');
		l = l->next;
	}
	write_file(c->filename, data);
	c->sts.changed = false;
	data.length--;
	raze_ddString(&data);
}

void vidd_enter_normal_mode(struct client* c)
	{ c->mode = VIDD_MODE_NORMAL; vidd_draw_status(c); };
void vidd_enter_insert_mode(struct client* c)
	{ c->mode = VIDD_MODE_INSERT; vidd_draw_status(c); };
void vidd_enter_replace_mode(struct client* c)
	{ c->mode = VIDD_MODE_REPLACE; vidd_draw_status(c); };
void vidd_enter_line_select_mode(struct client* c)
{
	c->mode = VIDD_MODE_LINE_SELECT;
	vidd_draw_status(c);
	c->sel.y1 = c->cur.y;
	c->sel.x1 = c->cur.x;
}
void vidd_center(struct client* c)
{
	if (c->cur.y->num > c->height/2)
	{
		long linecount = line_get_last(c->cur.y)->num+1;
		c->spos = c->cur.y->num-(c->height/2);
		if (c->spos > linecount)
		{
			c->spos = linecount;
		}
		else
		{
			vidd_redraw(*c);
			cursor_move_to(c->cur.x, c->height/2);
		}
	}
	else
	{
		c->spos = 0;
		vidd_redraw(*c);
	}
}
void vidd_indent(struct client* c)
{
	line_insert(c->cur.y, 0, ' ');
	line_insert(c->cur.y, 0, ' ');
	line_insert(c->cur.y, 0, ' ');
	line_insert(c->cur.y, 0, ' ');
	vidd_redraw_line(*c);
	c->cur.x += 4;
	cursor_right();
	cursor_right();
	cursor_right();
	cursor_right();
}
void vidd_deindent(struct client* c)
{
	if (*((int*)(c->cur.y->text)) != *((int*)"    ")) return;
	line_delete(c->cur.y, 0);
	line_delete(c->cur.y, 0);
	line_delete(c->cur.y, 0);
	line_delete(c->cur.y, 0);
	vidd_redraw_line(*c);
	c->cur.x -= 4;
	cursor_left();
	cursor_left();
	cursor_left();
	cursor_left();
}
void vidd_goto_top(struct client* c)
{
	struct line* l = line_get_first(c->cur.y);
	c->spos = l->num;
	c->cur.y = l;
	vidd_redraw(*c);
	c->cur.x = 0;
	cursor_home();
	cursor_return();
}
void vidd_goto_bottom(struct client* c)
{
	struct line* l = c->cur.y;
	while (l->next) l = l->next;
	c->spos = l->num;
	c->cur.y = l;
	vidd_redraw(*c);
	c->cur.x = 0;
	cursor_home();
	cursor_return();
}

void vidd_insert_at_start(struct client* c)
{
	vidd_goto_start(c);
	vidd_enter_insert_mode(c);
}
void vidd_insert_at_end(struct client* c)
{
	vidd_goto_end(c);
	vidd_right_insert(c);
}
void vidd_start_macro(struct client* c)
{
	if (!c->mac.recording)
	{
		c->mac.recording = true;
		if (c->mac.data) free(c->mac.data);
		c->mac.data = malloc(100);
		c->mac.cap = 100;
		c->mac.len = 0;
		c->mac.recording = true;
	}
	else
	{
		c->mac.recording = false;
	}
}
void vidd_run_macro(struct client* c)
{
	for (int i = 0; i < c->mac.len; i++)
		vidd_handel_key(c->mac.data[i]);
	vidd_enter_normal_mode(c);
}
void vidd_enter_select_mode(struct client* c)
{
	c->mode = VIDD_MODE_SELECT;
	vidd_draw_status(c);
	c->sel.y1 = c->cur.y;
	c->sel.x1 = c->cur.x;
}

void vidd_draw_status(struct client* c)
{
	cursor_save();
	vidd_draw_status_start(c);
	cursor_restore();

}
void vidd_draw_status_start(struct client* c)
{
	switch (c->mode)
	{
		case VIDD_MODE_NORMAL:
		{
			cursor_move_to(0, c->height);
			cursor_erase_line();
			ddPrintf("[NORMAL]");
		} break;
		case VIDD_MODE_INSERT:
		{
			cursor_move_to(0, c->height);
			cursor_erase_line();
			ddPrintf("[INSERT]");
		} break;
		case VIDD_MODE_REPLACE:
		{
			cursor_move_to(0, c->height);
			cursor_erase_line();
			ddPrintf("[REPLACE]");
		} break;
		case VIDD_MODE_SELECT:
		{
			cursor_move_to(0, c->height);
			cursor_erase_line();
			ddPrintf("[SELECT]");
		} break;
		case VIDD_MODE_LINE_SELECT:
		{
			cursor_move_to(0, c->height);
			cursor_erase_line();
			ddPrintf("[LSELECT]");
		} break;
	}
}



void vidd_goto_end(struct client* c)
{
	while (c->cur.x+1 < c->cur.y->len)
		vidd_move_right(c);
}
void vidd_goto_start(struct client* c)
{
	while (c->cur.x > 0)
		vidd_move_left(c);
	if (*((int*)(c->cur.y->text)) == *((int*)"	"))
	{
		vidd_skip_word(c);
	}
}
void vidd_delete_line(struct client* c)
{
	c->sts.changed = true;
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
		line_adjust_num_after(n, -1);
		c->cur.y = n;
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
	if (IS_LETTER(c->cur.y->text[c->cur.x]))
	{
		while (c->cur.x+1 < c->cur.y->len && IS_LETTER(c->cur.y->text[c->cur.x]))
			vidd_move_right(c);
	}
	else
	{
		char on = c->cur.y->text[c->cur.x];
		while (c->cur.x+1 < c->cur.y->len && c->cur.y->text[c->cur.x] == on)
			vidd_move_right(c);
	}
}
void vidd_skip_word_back(struct client* c)
{
	if (IS_LETTER(c->cur.y->text[c->cur.x]))
	{
		while (c->cur.x > 0 && IS_LETTER(c->cur.y->text[c->cur.x]))
			vidd_move_left(c);
	}
	else
	{
		char on = c->cur.y->text[c->cur.x];
		while (c->cur.x > 0 && c->cur.y->text[c->cur.x] == on)
			vidd_move_left(c);
	}
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

void vidd_copy_line(struct client* c)
{
	if (!c->buf.data) free(c->buf.data);
	c->buf.data = malloc(c->cur.y->len+1);
	c->buf.len = c->cur.y->len+1;
	mem_copy(c->buf.data+1, c->cur.y->text, c->cur.y->len);
	c->buf.data[0] = 'o';
}
void vidd_copy(struct client* c)
{
	c->sel.y2 = c->cur.y;
	c->sel.x2 = c->cur.x;
	ddString v;
	if (c->mode == VIDD_MODE_SELECT)
	{
		v = make_ddString_capacity("a", ABS(c->sel.y1->num-c->sel.y2->num)*100);
		if (c->sel.y2->num == c->sel.y1->num)
		{
			struct line* l = c->sel.y1;
			for (int i = c->sel.x1; i < c->sel.x2; i++)
				ddString_push_char_back(&v, l->text[i]);
		}
		else if (c->sel.y2->num > c->sel.y1->num)
		{
			struct line* l = c->sel.y1;
			for (int i = c->sel.x1; i < l->len; i++)
				ddString_push_char_back(&v, l->text[i]);
			ddString_push_char_back(&v, '\n');
			l = l->next;
			while (l->num != c->sel.y2->num)
			{
				for (int i = 0; i < l->len; i++)
					ddString_push_char_back(&v, l->text[i]);
				ddString_push_char_back(&v, '\n');
				l = l->next;
			}
			for (int i = 0; i < c->sel.x2; i++)
				ddString_push_char_back(&v, l->text[i]);
		}
		else
		{
			struct line* l = c->sel.y2;
			for (int i = c->sel.x2; i < l->len; i++)
				ddString_push_char_back(&v, l->text[i]);
			ddString_push_char_back(&v, '\n');
			l = l->next;
			while (l->num != c->sel.y1->num)
			{
				for (int i = 0; i < l->len; i++)
					ddString_push_char_back(&v, l->text[i]);
				ddString_push_char_back(&v, '\n');
				l = l->next;
			}
			for (int i = 0; i < c->sel.x1; i++)
				ddString_push_char_back(&v, l->text[i]);
		}
	}
	else if (c->mode == VIDD_MODE_LINE_SELECT)
	{
		v = make_ddString("o");
		if (c->sel.y2->num == c->sel.y1->num)
		{
			struct line* l = c->sel.y1;
			for (int i = 0; i < l->len; i++)
				ddString_push_char_back(&v, l->text[i]);
		}
		else if (c->sel.y2->num > c->sel.y1->num)
		{
			struct line* l = c->sel.y1;
			for (int i = 0; i < l->len; i++)
				ddString_push_char_back(&v, l->text[i]);
			ddString_push_char_back(&v, '\n');
			l = l->next;
			while (l->num != c->sel.y2->num)
			{
				for (int i = 0; i < l->len; i++)
					ddString_push_char_back(&v, l->text[i]);
				ddString_push_char_back(&v, '\n');
				l = l->next;
			}
			for (int i = 0; i < l->len; i++)
				ddString_push_char_back(&v, l->text[i]);
		}
		else
		{
			struct line* l = c->sel.y2;
			for (int i = c->sel.x2; i < l->len; i++)
				ddString_push_char_back(&v, l->text[i]);
			ddString_push_char_back(&v, '\n');
			l = l->next;
			while (l->num != c->sel.y1->num)
			{
				for (int i = 0; i < l->len; i++)
					ddString_push_char_back(&v, l->text[i]);
				ddString_push_char_back(&v, '\n');
				l = l->next;
			}
			for (int i = 0; i < c->sel.x1; i++)
				ddString_push_char_back(&v, l->text[i]);
		}
	}
	if (!c->buf.data) free(c->buf.data);
	c->buf.len = v.length;
	c->buf.data = v.cstr;
	vidd_enter_normal_mode(c);
}
void vidd_paste_line(struct client* c)
{
	for (int i = 0; i < c->buf.len; i++)
		vidd_handel_key(c->buf.data[i]);
	vidd_enter_normal_mode(c);
	vidd_move_left(c);
}

void vidd_adjust_x(struct client* c)
{
	if (c->cur.x == 0) return;
	if (c->cur.y->len == 0)
	{
		vidd_goto_start(c);
		return;
	}
	if (c->cur.x >= c->cur.y->len)
	{
		long epos = c->cur.y->len-1;
		vidd_goto_start(c);
		while (c->cur.x != epos) vidd_move_right(c);
	}
}

void vidd_move_force_right(struct client* c)
{
	c->cur.x++;
	if (c->cur.x-c->vpos >= c->width)
	{
		c->vpos++;
		vidd_redraw(*c);
	}
	else cursor_right();
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
			cursor_up();
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
		if (c->cur.x-c->vpos >= c->width)
		{
			c->vpos++;
			vidd_redraw(*c);
		}
		else cursor_right();
	}
}
void vidd_move_left(struct client* c)
{
	if (c->cur.x > 0)
	{
		c->cur.x--;
		if (c->cur.x-c->vpos < 0)
		{
			c->vpos--;
			vidd_redraw(*c);
		}
		cursor_left();
	}
}

void vidd_delete(struct client* c)
{
	if (c->cur.y->len == 0) return;
	c->sts.changed = true;
	vidd_adjust_x(c);
	line_delete(c->cur.y, c->cur.x);
	vidd_redraw_line(*c);
	vidd_adjust_x(c);
}
void vidd_backspace(struct client* c)
{
	if (c->cur.x == 0) return;
	c->sts.changed = true;
	line_delete(c->cur.y, c->cur.x-1);
	vidd_redraw_line(*c);
	vidd_move_left(c);
}

void vidd_redraw_line(struct client c)
{
	cursor_save();

	cursor_erase_line();

	cursor_return();
	ddPrint(c.cur.y->text+c.vpos, CLAMP(c.cur.y->len, c.width));

	cursor_restore();
}
void vidd_redraw(struct client c)
{
	vidd_print(c);
}
void vidd_print(struct client c)
{
	cursor_save();
	cursor_clear();
	cursor_home();
	struct line* l = line_get_line(c.cur.y, c.spos);
	for (int h = 0; h < c.height-1 && l; h++)
	{
		//ddPrintf("%d ", l->num);
		if (c.vpos < l->len)
		{
			for (int i = 0; i < CLAMP(l->len-c.vpos, c.width); i++)
			{
				for (int s = 0; styles[0].keywords[s].len != 0; s++)
				{
					if ((((!IS_LETTER(l->text[i+c.sty.keywords[s].len]) ||
									i+c.vpos+c.sty.keywords[s].len >= l->len) &&
								(i+c.vpos-1 < c.vpos || !IS_LETTER(l->text[i+c.vpos-1]))) ||
							!c.sty.keywords[s].embedded) &&
						cstring_compare_length(c.sty.keywords[s].word, &l->text[i+c.vpos], c.sty.keywords[s].len))
					{
						ddPrints(c.sty.keywords[s].style);
						if (i+c.vpos + c.sty.keywords[s].len > c.width)
							ddPrint(&l->text[i+c.vpos], c.width-(i+c.vpos));
						else ddPrint(&l->text[i+c.vpos], c.sty.keywords[s].len);
						ddPrints(CWHITE);
						i += c.sty.keywords[s].len;
						break;
					}
				}
				if (i < CLAMP(l->len-c.vpos, c.width)) ddPrint_char(l->text[i+c.vpos]);
			}
		}
		ddPrint_nl();
		l = l->next;
	}
	cursor_restore();
	vidd_draw_status(&c);
}

void vidd_handel_key(char key)
{
	switch (cmaster.mode)
	{
		case VIDD_MODE_SELECT:
		case VIDD_MODE_LINE_SELECT:
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
			cmaster.sts.changed = true;
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
			cmaster.sts.changed = true;
			line_insert(cmaster.cur.y, cmaster.cur.x, key);
			vidd_redraw_line(cmaster);
			vidd_move_force_right(&cmaster);
			if (key == '	')
			{
				vidd_move_force_right(&cmaster);
				vidd_move_force_right(&cmaster);
				vidd_move_force_right(&cmaster);
			}
		} break;
	}
}

void vidd_run_command(struct client* c, char* com, char* comdat)
{
	for (int i = 0; i < sizeof(commands)/sizeof(struct command); i++)
	{
		if (cstring_compare(com, commands[i].com))
			commands[i].function(c, comdat);
	}
}

void vidd_read_command(struct client* c)
{
	char comdat[2][100] = {0};
	long comlen[2] = {0, 0};
	int comidx = 0;
	char key = 0;

	cursor_save();
	vidd_draw_status_start(c);
	ddPrints(":");
	while ((key = ddKey_getch_noesc()) != '\n' && key != 27)
	{
		if (key == DDK_BACKSPACE && comlen[comidx] > 0)
		{
			comdat[comidx][--comlen[comidx]] = 0;
			cursor_left();
			ddPrint_char(' ');
			cursor_left();
			continue;
		}
		if (key == ' ')
		{
			comidx++;
			continue;
		}
		comdat[comidx][comlen[comidx]++] = key;
		ddPrint_char(key);
	}
	cursor_restore();
	vidd_draw_status(c);
	if (key != 27) 
		vidd_run_command(c, comdat[0], comdat[1]);
}

void vidd_macro_insert(struct client* c, char key)
{
	if (c->mac.len+1 > c->mac.cap)
	{
		c->mac.data = realloc(c->mac.data, c->mac.len+10);
		c->mac.cap = c->mac.len+10;
	}
	c->mac.data[c->mac.len++] = key;
}

int vidd_main(void)
{
	for (;;)
	{
		char key = ddKey_getch_noesc();
		if (cmaster.mac.recording) vidd_macro_insert(&cmaster, key);
		vidd_handel_key(key);
	}
}

int main(int argc, char** argv)
{
	ddString data = read_file(argv[1]);
	data.length++;

	cmaster.filename = argv[1];

	cmaster.width = cursor_get_width();
	cmaster.height = cursor_get_height();
	cmaster.x = 0;
	cmaster.y = 0;

	cmaster.mac.recording = false;

	cmaster.mode = VIDD_MODE_NORMAL;

	cmaster.buf.data = 0;

	cmaster.sty = styles[0];

	cmaster.sts.changed = false;

	cmaster.cur.x = 0;
	cmaster.cur.y = make_line(0, 0, 0, 100);
	cmaster.spos = 0;
	cmaster.vpos = 0;

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
	vidd_enter_normal_mode(&cmaster);
	cursor_home();

	cmaster.cur.y = line_get_line(cmaster.cur.y, cmaster.spos);

	vidd_main();

	

	return 0;
}

#endif
