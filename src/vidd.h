#ifndef __vidd_h__
#define __vidd_h__

#include <ddcDef.h>
#include <ddcString.h>
#include <ddcCursor.h>
#include <ddcVec.h>
#include <ddcColors.h>
#include <ddcKeyboard.h>

struct vidd;
struct viddCursor;
struct viddBuffer;
struct viddCommand;

enum viddWriteMode{ VIDD_MODE_NORMAL=0, VIDD_MODE_INSERT, VIDD_MODE_REPLACE};

struct viddCommand
{
	const char* name;
	void (*function)(struct vidd*);
};

#include "./commands.h"

struct viddCursor
{
	ddIVec2 position;
	ddColor fgColor;
	ddColor bgColor;
};

struct viddBuffer
{
	ddString* data;
	int lineCount;
	int height;
	int width;
};

struct vidd
{
	struct viddBuffer buffer;
	struct viddCursor cursor;
	enum viddWriteMode mode;
};

struct viddBuffer make_viddBuffer(int width, int height);
struct viddCursor make_viddCursor(ddIVec2 position, ddColor fg, ddColor bg);
struct vidd make_vidd(int width, int height);

void vidd_reset_line_nulls(struct vidd* vidd, uint16t line);// puts nulls ("\0") at the begining of a line
void vidd_reset_nulls(struct vidd* vidd);
void vidd_clear(struct vidd* vidd);
void vidd_clear_line(struct vidd* vidd, uint16t line);
void vidd_buffer_replace(struct vidd* vidd, char key);
void vidd_set_mode(struct vidd* vidd, int mode);
void vidd_set_position(struct vidd* vidd, int x, int y);
void vidd_write_char(struct vidd* vidd, char key);
void vidd_draw_mode(struct vidd* vidd);
void vidd_redraw_line_at(struct vidd* vidd, int position);
int vidd_get_last_nonwhite(struct vidd* vidd);
int vidd_get_first_nonwhite(struct vidd* vidd);
void vidd_handle_normal_mode_key(struct vidd* vidd, char key);
void vidd_handle_insert_mode_key(struct vidd* vidd, char key);
void vidd_handle_replace_mode_key(struct vidd* vidd, char key);
void vidd_read_command(struct vidd* vidd);
void vidd_run_command(struct vidd* vidd, ddString command);
void vidd_main_input_loop(struct vidd* vidd);

struct viddBuffer make_viddBuffer(int width, int height)
{
	struct viddBuffer output;
	output.data = make(ddString, height);
	output.data[0] = make_ddString("");
	output.lineCount = 1;
	output.height = height;
	output.width = width;
	return output;
}

struct viddCursor make_viddCursor(ddIVec2 position, ddColor fg, ddColor bg)
{
	struct viddCursor output;
	output.position = position;
	output.fgColor = fg;
	output.bgColor = bg;
	return output;
}

struct vidd make_vidd(int width, int height)
{
	struct vidd output;
	output.buffer = make_viddBuffer(width, height);
	output.cursor = make_viddCursor(make_ddIVec2(0, 0), make_ddColor(255,255,255), make_ddColor(0,0,0));
	output.mode = VIDD_MODE_NORMAL;
	return output;
}







void vidd_set_position(struct vidd* vidd, int x, int y)
{
	if (x >= 0 && x < vidd->buffer.data[vidd->cursor.position.y].length &&
	    y >= 0 && y < vidd->buffer.lineCount)
	{
		vidd->cursor.position.x = x;
		vidd->cursor.position.y = y;
		cursor_move_to(x, y);
	}
}
void vidd_write_char(struct vidd* vidd, char key)
{
	vidd->cursor.position.x++;
	cursor_write_char(key);
}


void vidd_draw_mode(struct vidd* vidd)
{
	cursor_push();
	if (vidd->mode == VIDD_MODE_NORMAL)
	{
		cursor_move_to(0, vidd->buffer.height);
		cursor_delete_line();
		cursor_write_cstring("[NORMAL]");
	}
	else if (vidd->mode == VIDD_MODE_INSERT)
	{
		cursor_move_to(0, vidd->buffer.height);
		cursor_delete_line();
		cursor_write_cstring("[INSERT]");
	}
	else if (vidd->mode == VIDD_MODE_REPLACE)
	{
		cursor_move_to(0, vidd->buffer.height);
		cursor_delete_line();
		cursor_write_cstring("[REPALCE]");
	}
	cursor_pop();
}

int vidd_get_last_nonwhite(struct vidd* vidd)
{
	int output = 0;
	for (int i = 0; i < vidd->buffer.width; i++)
		if (vidd->buffer.data[vidd->cursor.position.y].cstr[i] != ' ') output = i;
	return output;
}
int vidd_get_first_nonwhite(struct vidd* vidd)
{
	int output;
	for (output = 0; vidd->buffer.data[vidd->cursor.position.y].cstr[output] == ' '; output++)
	return output;
}

void vidd_redraw_line_at(struct vidd* vidd, int position)
{
	cursor_push();
	cursor_move_to(vidd->buffer.data[vidd->cursor.position.y].length, vidd->cursor.position.y);
	cursor_write_char(' ');
	cursor_move_to(vidd->cursor.position.x, vidd->cursor.position.y);
	for (int i = position; i < vidd->buffer.data[vidd->cursor.position.y].length; i++)
		cursor_write_char(vidd->buffer.data[vidd->cursor.position.y].cstr[i]);
	cursor_pop();
}

void vidd_set_mode(struct vidd* vidd, int mode)
{
	vidd->mode = mode;
	vidd_draw_mode(vidd);
}

void vidd_clear(struct vidd* vidd)
{
	for (int y = 0; y < vidd->buffer.height; y++)
		vidd_clear_line(vidd, y);
	vidd_reset_nulls(vidd);
	vidd_set_position(vidd, 0, 0);
}
void vidd_clear_line(struct vidd* vidd, uint16t line)
{
	for (int x = 0; x < vidd->buffer.width; x++)
		vidd->buffer.data[line].cstr[x] = '\0';
	vidd_reset_line_nulls(vidd, line);
	cursor_delete_line();
}

void vidd_reset_line_nulls(struct vidd* vidd, uint16t line)
{
	vidd->buffer.data[line].cstr[0] = '\0';
}
void vidd_reset_nulls(struct vidd* vidd)
{
	for (int y = 0; y < vidd->buffer.height; y++)
		vidd_reset_line_nulls(vidd, y);
}

void vidd_buffer_replace(struct vidd* vidd, char key)
{
	vidd->buffer.data[vidd->cursor.position.y].cstr[vidd->cursor.position.x] = key;
}
void vidd_buffer_insert(struct vidd* vidd, char key)
{
	ddString_insert_char_at(&(vidd->buffer.data[vidd->cursor.position.y]), key, vidd->cursor.position.x);
	vidd_redraw_line_at(vidd, vidd->cursor.position.x);
	vidd->cursor.position.x++;
	cursor_move_right();
}

void vidd_main_input_loop(struct vidd* vidd)
{
	cursor_clear();
	vidd_set_mode(vidd, VIDD_MODE_NORMAL);
	char key;
	for(;;)
	{
		key = ddKey_getch_noesc();
		if (vidd->mode == VIDD_MODE_NORMAL)
		{
			vidd_handle_normal_mode_key(vidd, key);
		}
		else if (vidd->mode == VIDD_MODE_INSERT)
		{
			vidd_handle_insert_mode_key(vidd, key);
		}
		else if (vidd->mode == VIDD_MODE_REPLACE)
		{
			vidd_handle_replace_mode_key(vidd, key);
		}
	}
}

void vidd_read_command(struct vidd* vidd)
{
	ddString command = make_ddString("");
	cursor_push();
	cursor_move_to(0, vidd->buffer.height-1);
	ddPrint_char(':');
	char comKey = ddKey_getch_noesc();
	while (comKey != DDK_RETURN && comKey != DDK_ESCAPE)
	{
		ddString_push_char_back(&command, comKey);
		cursor_write_char(comKey);
		comKey = ddKey_getch_noesc();
	}
	cursor_delete_line();
	cursor_pop();
	if (comKey != DDK_ESCAPE)
		vidd_run_command(vidd, command);
	raze_ddString(&command);
}

void vidd_run_command(struct vidd* vidd, ddString command)
{
	for (sizet i = 0; i < viddCommandVectorLength; i++)
	{
		if (ddString_compare_cstring(command, viddCommandVector[i].name))
		{
			(*(viddCommandVector[i].function))(vidd);
			return;
		}
	}
}

void vidd_handle_normal_mode_key(struct vidd* vidd, char key)
{
	switch (key)
	{
		case ':':
		{
			vidd_read_command(vidd);
			break;
		}
		case 'o':
			//vidd_insert_line(vidd, vidd->cursor.position.y+1);
			break;
		case 'O':
			//vidd_insert_line(vidd, vidd->cursor.position.y-1);
			break;
		case 'i':
			vidd_set_mode(vidd, VIDD_MODE_INSERT);
			break;
		case 'a':
			vidd_set_mode(vidd, VIDD_MODE_INSERT);
			cursor_move(1, 0);
			vidd->cursor.position.x++;
			break;
		case 'q':
			cursor_clear();
			exit(0);
			break;
		case '$':
		{
			int pos = vidd->buffer.data[vidd->cursor.position.y].length-1;
			cursor_move_to(pos, vidd->cursor.position.y);
			vidd->cursor.position.x = pos;
			break;
		}
		case '^':
		{
			int pos = vidd_get_first_nonwhite(vidd);
			cursor_move_to(pos, vidd->cursor.position.y);
			vidd->cursor.position.x = pos;
			vidd->cursor.position.y = vidd->cursor.position.y;
			break;
		}
		case 'x':
			ddString_delete_at(&(vidd->buffer.data[vidd->cursor.position.y]), vidd->cursor.position.x);
			vidd_redraw_line_at(vidd, vidd->cursor.position.x);
			if (vidd->cursor.position.x >= vidd->buffer.data[vidd->cursor.position.y].length)
			{
				vidd->cursor.position.x--;
				cursor_move_left();
			}
			break;
		case 'X':
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			ddString_delete_at(&(vidd->buffer.data[vidd->cursor.position.y]), vidd->cursor.position.x);
			vidd_write_char(vidd, ' ');
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_redraw_line_at(vidd, vidd->cursor.position.x);
			break;
		case 'd':
		{// do this better yo
			char nextKey = ddKey_getch_noesc();
			if (nextKey == 'd')
			{
				vidd_clear_line(vidd, vidd->cursor.position.y);
				//viddCursor_set_position(&(vidd->cursor), 0, vidd->cursor.position.y);
				// pop line
			}
			break;
		}
		case 'r':
			vidd_handle_replace_mode_key(vidd, ddKey_getch_noesc());
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			break;
		case 'R':
			vidd_set_mode(vidd, VIDD_MODE_REPLACE);
			break;
		case 'h':
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			break;
		case 'j':
			vidd_set_position(vidd, vidd->cursor.position.x, vidd->cursor.position.y+1);
			break;
		case 'k':
			vidd_set_position(vidd, vidd->cursor.position.x, vidd->cursor.position.y-1);
			break;
		case 'l':
			vidd_set_position(vidd, vidd->cursor.position.x+1, vidd->cursor.position.y);
			break;
	}
}

void vidd_handle_insert_mode_key(struct vidd* vidd, char key)
{
	switch (key)
	{
		case DDK_ESCAPE:
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_set_mode(vidd, VIDD_MODE_NORMAL);
			break;
		case DDK_BACKSPACE:
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			ddString_delete_at(&(vidd->buffer.data[vidd->cursor.position.y]), vidd->cursor.position.x);
			vidd_write_char(vidd, ' ');
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_redraw_line_at(vidd, vidd->cursor.position.x);
			break;
		default:
			vidd_buffer_insert(vidd, key);
			break;
	}
}
void vidd_handle_replace_mode_key(struct vidd* vidd, char key)
{
	switch (key)
	{
		case DDK_ESCAPE:
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_set_mode(vidd, VIDD_MODE_NORMAL);
			break;
		case DDK_BACKSPACE:
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			ddString_delete_at(&(vidd->buffer.data[vidd->cursor.position.y]), vidd->cursor.position.x);
			vidd_write_char(vidd, ' ');
			vidd_set_position(vidd, vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_redraw_line_at(vidd, vidd->cursor.position.x);
			break;
		default:
			vidd_buffer_replace(vidd, key);
			vidd_write_char(vidd, key);
			break;
	}
}

#endif
