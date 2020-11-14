#ifndef __vidd_h__
#define __vidd_h__

#include <ddcDef.h>
#include <ddcString.h>
#include <ddcCursor.h>
#include <ddcVec.h>
#include <ddcColors.h>
#include <ddcKeyboard.h>

enum viddWriteMode{ VIDD_MODE_NORMAL=0, VIDD_MODE_INSERT, VIDD_MODE_REPLACE};

struct viddCursor
{
	ddIVec2 position;
	ddColor fgColor;
	ddColor bgColor;
};

struct vidd
{
	ddString* buffer;
	enum viddWriteMode mode;
	struct viddCursor cursor;
	uint16t height;
	uint16t width;
};

struct viddCursor make_viddCursor(ddIVec2 position, ddColor fg, ddColor bg);
struct viddCursor viddCursor_set_position(struct viddCursor* vcursor, int x, int y);
struct viddCursor viddCursor_write_char(struct viddCursor* vcursor, char key);

struct vidd make_vidd(uint16t width, uint16t height);
void vidd_reset_line_nulls(struct vidd* vidd, uint16t line);// puts nulls ("\0") at the begining of a line
void vidd_reset_nulls(struct vidd* vidd);
void vidd_clear(struct vidd* vidd);
void vidd_clear_line(struct vidd* vidd, uint16t line);
void vidd_buffer_replace(struct vidd* vidd, char key);
void vidd_set_mode(struct vidd* vidd, int mode);
void vidd_draw_mode(struct vidd* vidd);
void vidd_handle_normal_mode_key(struct vidd* vidd, char key);
void vidd_handle_insert_mode_key(struct vidd* vidd, char key);
void vidd_handle_replace_mode_key(struct vidd* vidd, char key);
void vidd_main_input_loop(struct vidd* vidd);

struct viddCursor make_viddCursor(ddIVec2 position, ddColor fg, ddColor bg)
{
	struct viddCursor output;
	output.position = position;
	output.fgColor = fg;
	output.bgColor = bg;
	return output;
}

struct viddCursor viddCursor_set_position(struct viddCursor* vcursor, int x, int y)
{
	vcursor->position.x = x;
	vcursor->position.y = y;
	cursor_moveTo(x, y);
}
struct viddCursor viddCursor_write_char(struct viddCursor* vcursor, char key)
{
	vcursor->position.x++;
	ddPrint_char(key);
}


struct vidd make_vidd(uint16t width, uint16t height)
{
	struct vidd output;
	output.height = height;
	output.width = width;
	output.buffer = make(ddString, height);
	for (uint16t y = 0; y < height; y++)
		output.buffer[y] = make_multi_ddString_cstring(" ", width);
	output.mode = VIDD_MODE_NORMAL;
	output.cursor = make_viddCursor(make_ddIVec2(0, 0), make_ddColor(255,255,255), make_ddColor(0,0,0));
	return output;
}

void vidd_draw_mode(struct vidd* vidd)
{
	cursor_push();

	if (vidd->mode == VIDD_MODE_NORMAL)
	{
		cursor_moveTo(0, vidd->height);
		cursor_deleteLine();
		cursor_chWrite("[NORMAL]");
	}
	else if (vidd->mode == VIDD_MODE_INSERT)
	{
		cursor_moveTo(0, vidd->height);
		cursor_deleteLine();
		cursor_chWrite("[INSERT]");
	}
	else if (vidd->mode == VIDD_MODE_REPLACE)
	{
		cursor_moveTo(0, vidd->height);
		cursor_deleteLine();
		cursor_chWrite("[REPALCE]");
	}

	cursor_pop();
	cursor_moveUp();
	cursor_moveLeft();
}

void vidd_set_mode(struct vidd* vidd, int mode)
{
	vidd->mode = mode;
	vidd_draw_mode(vidd);
}

void vidd_clear(struct vidd* vidd)
{
	for (int y = 0; y < vidd->height; y++)
		vidd_clear_line(vidd, y);
	vidd_reset_nulls(vidd);
	viddCursor_set_position(&(vidd->cursor), 0, 0);
}
void vidd_clear_line(struct vidd* vidd, uint16t line)
{
	for (int x = 0; x < vidd->width; x++)
		vidd->buffer[line].cstr[x] = ' ';
	vidd_reset_line_nulls(vidd, line);
	cursor_deleteLine();
}

void vidd_reset_line_nulls(struct vidd* vidd, uint16t line)
{
	vidd->buffer[line].cstr[0] = '\0';
}
void vidd_reset_nulls(struct vidd* vidd)
{
	for (int y = 0; y < vidd->height; y++)
		vidd_reset_line_nulls(vidd, y);
}

void vidd_buffer_replace(struct vidd* vidd, char key)
{
	vidd->buffer[vidd->cursor.position.y].cstr[vidd->cursor.position.x] = key;
}

void vidd_main_input_loop(struct vidd* vidd)
{
	init_cursor();
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

void vidd_handle_normal_mode_key(struct vidd* vidd, char key)
{
	switch (key)
	{
		case ':':
		{
			ddString command = make_ddString("");
			char comKey = ddKey_getch_noesc();
			cursor_push();
			cursor_moveTo(0, vidd->height-1);
			ddPrint_char(':');
			while (comKey != DDK_RETURN && comKey != DDK_ESCAPE)
			{
				ddString_push_char_back(&command, comKey);
				ddPrint_char(comKey);
				comKey = ddKey_getch_noesc();
			}
			cursor_deleteLine();
			cursor_pop();
			cursor_moveUp();
			cursor_moveLeft();
			raze_ddString(&command);
			break;
		}
		case 'i':
			vidd_set_mode(vidd, VIDD_MODE_INSERT);
			break;
		case 'q':
			cursor_clear();
			exit(0);
			break;
		case 'x':
			//pop character
			break;
		case 'X':
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_buffer_replace(vidd, ' ');
			viddCursor_write_char(&(vidd->cursor), ' ');
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
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
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			break;
		case 'R':
			vidd_set_mode(vidd, VIDD_MODE_REPLACE);
			break;
		case 'h':
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			break;
		case 'j':
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x, vidd->cursor.position.y+1);
			break;
		case 'k':
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x, vidd->cursor.position.y-1);
			break;
		case 'l':
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x+1, vidd->cursor.position.y);
			break;
	}
}

void vidd_handle_insert_mode_key(struct vidd* vidd, char key)
{
	switch (key)
	{
		case DDK_ESCAPE:
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_set_mode(vidd, VIDD_MODE_NORMAL);
			break;
		case DDK_BACKSPACE:
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_buffer_replace(vidd, ' ');
			viddCursor_write_char(&(vidd->cursor), ' ');
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			break;
		default:
			//vidd_buffer_insert(vidd, key);
			vidd_buffer_replace(vidd, key);
			viddCursor_write_char(&(vidd->cursor), key);
			break;
	}
}
void vidd_handle_replace_mode_key(struct vidd* vidd, char key)
{
	switch (key)
	{
		case DDK_ESCAPE:
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_set_mode(vidd, VIDD_MODE_NORMAL);
			break;
		case DDK_BACKSPACE:
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			vidd_buffer_replace(vidd, ' ');
			viddCursor_write_char(&(vidd->cursor), ' ');
			viddCursor_set_position(&(vidd->cursor), vidd->cursor.position.x-1, vidd->cursor.position.y);
			break;
		default:
			vidd_buffer_replace(vidd, key);
			viddCursor_write_char(&(vidd->cursor), key);
			break;
	}
}

#endif
