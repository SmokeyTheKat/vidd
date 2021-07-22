#ifndef __VIDD_COMMANDS_H__
#define __VIDD_COMMANDS_H__

#include "./syntax.h"

void vidd_redraw(struct vidd_client* client);
void vidd_redraw_line(struct vidd_client* client);
void vidd_toggle_drawing(struct vidd_client* client);

void vidd_check_for_window_size_change(struct vidd_client* client);

void vidd_set_status(struct vidd_client* client);

bool vidd_view_fix_overflow(struct vidd_client* client);
bool vidd_cursor_fix_overflow(struct vidd_client* client);
bool vidd_selection_redraw_needed(struct vidd_client* client);
void vidd_cursor_adjust(struct vidd_client* client);

void vidd_text_clear(struct vidd_client* client);

intmax_t vidd_last_movable(struct vidd_client* client);
int vidd_cursor_is_out_of_view(struct vidd_client* client);
intmax_t vidd_view_get_absolute_x_offset(struct vidd_client* client);
intmax_t vidd_view_get_absolute_y_offset(struct vidd_client* client);

void vidd_cursor_center(struct vidd_client* client);

void vidd_move_up(struct vidd_client* client);
void vidd_move_down(struct vidd_client* client);
void vidd_move_right(struct vidd_client* client);
void vidd_move_left(struct vidd_client* client);
void vidd_move_to_line_start(struct vidd_client* client);
void vidd_move_to_line_end(struct vidd_client* client);
void vidd_move_next_word(struct vidd_client* client);
void vidd_move_next_word_end(struct vidd_client* client);
void vidd_move_prev_word(struct vidd_client* client);
void vidd_move_to_spot_in_view_after(struct vidd_client* client, intmax_t pos);

void vidd_view_adjust_offset(struct vidd_client* client);

void vidd_view_center_y(struct vidd_client* client);
void vidd_view_center_x(struct vidd_client* client);
void vidd_view_center(struct vidd_client* client);

void vidd_view_move_right(struct vidd_client* client);
void vidd_view_move_left(struct vidd_client* client);
void vidd_view_move_up(struct vidd_client* client);
void vidd_view_move_down(struct vidd_client* client);

void vidd_set_marker(struct vidd_client* client);
void vidd_goto_marker(struct vidd_client* client);

void vidd_goto_top(struct vidd_client* client);
void vidd_goto_bottom(struct vidd_client* client);
void vidd_move_to(struct vidd_client* client, intmax_t x, intmax_t y);

void vidd_frame_move_right(struct vidd_client* client);

void vidd_insert_char(struct vidd_client* client);

void vidd_new_line(struct vidd_client* client);
void vidd_insert_on_new_line(struct vidd_client* client);
void vidd_insert_on_new_line_up(struct vidd_client* client);

void vidd_indent(struct vidd_client* client);
void vidd_deindent(struct vidd_client* client);

void vidd_split_at_cursor(struct vidd_client* client);

void vidd_delete(struct vidd_client* client);
void vidd_delete_movement(struct vidd_client* client, char key);
void vidd_delete_selection(struct vidd_client* client);
void vidd_delete_line(struct vidd_client* client);
void vidd_backspace(struct vidd_client* client);
void vidd_delete_two_lines_up(struct vidd_client* client);
void vidd_delete_two_lines_down(struct vidd_client* client);

void vidd_replace_character(struct vidd_client* client);

void vidd_line_selection_draw(struct vidd_client* client);
void vidd_selection_draw(struct vidd_client* client);

void vidd_copy_line(struct vidd_client* client);
void vidd_selection_copy(struct vidd_client* client);

void vidd_paste(struct vidd_client* client);

void vidd_find_next_word(struct vidd_client* client, char* word, intmax_t length);

void vidd_enter_normal_mode(struct vidd_client* client);
void vidd_enter_insert_mode(struct vidd_client* client);
void vidd_enter_insert_mode_right(struct vidd_client* client);
void vidd_enter_select_mode(struct vidd_client* client);
void vidd_enter_line_select_mode(struct vidd_client* client);
void vidd_enter_command_mode(struct vidd_client* client);
void vidd_enter_find_next_mode(struct vidd_client* client);
void vidd_enter_find_prev_mode(struct vidd_client* client);
void vidd_enter_replace_mode(struct vidd_client* client);

void vidd_client_up(struct vidd_client* client);
void vidd_client_down(struct vidd_client* client);
void vidd_client_prev(struct vidd_client* client);
void vidd_client_next(struct vidd_client* client);
void vidd_client_left(struct vidd_client* client);
void vidd_client_right(struct vidd_client* client);
void vidd_swap_split(struct vidd_client* client);
void vidd_swap_active(void);
void vidd_swap(struct vidd_client* client);

void vidd_open_empty(struct vidd_client* client);

void vidd_edit(struct vidd_client* client, char* args);
void vidd_run_command_in_frame(struct vidd_client* client, char* args);
void vidd_split(struct vidd_client* client, char* args);
void vidd_vsplit(struct vidd_client* client, char* args);
void vidd_write(struct vidd_client* client, char* args);
void vidd_write_quit(struct vidd_client* client, char* args);
void vidd_client_quit(struct vidd_client* client, char* args);
void vidd_exit(struct vidd_client* client, char* args);

void vidd_void(struct vidd_client* client);





#include "./config.h"





void vidd_line_clear(struct vidd_client* client, intmax_t pos)
{
	char* fmt = INACTIVE_CLIENT_COLOR;
	if (client == vidd_get_active())
		fmt = ACTIVE_CLIENT_COLOR;

	char* line = malloc(client->width + 1);
	memset(line, ' ', client->width);
	line[client->width] = 0;

	cursor_move_to(client->x, client->y + pos);
	printf("%s%s" NOSTYLE, fmt, line);

	free(line);
}
void vidd_redraw(struct vidd_client* client)
{
	if (!client->displayOn) return;

	//vidd_check_for_window_size_change(client);
	vidd_view_adjust_offset(client);

	struct line* line = client->text;
	intmax_t line_count = line_get_last(line)->number;
	intmax_t line_number_gap = number_get_length(line_count);
	intmax_t visable_line_count = client->view.height;

	struct buffer toprint = make_buffer(client->view.height * client->view.width * 4);

	buffer_push_cstring(&toprint, NOSTYLE, sizeof(NOSTYLE)-1);

	if (client->view.y < 0)
	{
		buffer_printf(&toprint, CURSOR_TO("%d", "%d") FRGB("255", "255", "0") "*" NOSTYLE, client->y - client->view.y + 1 - 1, client->x+1);
		buffer_push_repeat(&toprint, ' ', client->width-1);
		buffer_push_cstring(&toprint, CURSOR_DOWN(), sizeof(CURSOR_DOWN())-1);
		buffer_printf(&toprint, CURSOR_TO("%d", "%d"), client->y - client->view.y + 1, client->x+1);

		visable_line_count += client->view.y;
	}
	else
	{
		line = line_skip(line, client->view.y);
		cursor_move_to(0, vidd_view_get_absolute_y_offset(client));
	}

	if (client->x != 0 && client->view.y >= 0)
		buffer_printf(&toprint, CURSOR_RIGHT("%d"), client->x);

	intmax_t i;
	for (i = 0; line && i < visable_line_count; i++)
	{
		buffer_push_repeat(&toprint, ' ', line_number_gap - number_get_length(line->number));

		buffer_printf(&toprint, FRGB("255", "255", "0") "%d│" NOSTYLE, line->number);

		if (client->view.xo > line_number_gap + 1)
			buffer_printf(&toprint, CURSOR_RIGHT("%d"), client->x);

		if (client->view.x < line->buffer.length)
			vidd_syntax_apply_to_buffer(client, &toprint, line);

		intmax_t empty_space = client->view.width - MAX((line->buffer.length - client->view.x), 0);
		empty_space = MIN(client->view.width, empty_space);
		buffer_push_repeat(&toprint, ' ', empty_space);

		if (i + 1 < visable_line_count)
			buffer_printf(&toprint, ((client->x != 0) ? ("\r" CURSOR_DOWN() CURSOR_RIGHT("%d")) : ("\r" CURSOR_DOWN())), client->x);
		line = line->next;
	}
	for (; i < visable_line_count; i++)
	{
		buffer_push_cstring(&toprint, FRGB("255", "255", "0"), sizeof(FRGB("255", "255", "0"))-1);
		buffer_push(&toprint, '*');
		buffer_push_repeat(&toprint, ' ', client->view.width - 1);
		if (i + 1 < visable_line_count)
			buffer_printf(&toprint, ((client->x != 0) ? ("\r" CURSOR_DOWN() CURSOR_RIGHT("%d")) : ("\r" CURSOR_DOWN())), client->x);
		buffer_push_cstring(&toprint, NOSTYLE, sizeof(NOSTYLE)-1);
	}

	printf("%s", toprint.data);

	free_buffer(&toprint);

	vidd_set_status(client);

	if (client->mode == VIDD_MODE_LINE_SELECT) vidd_selection_draw(client);
	if (client->mode == VIDD_MODE_SELECT) vidd_selection_draw(client);

	vidd_cursor_adjust(client);
}
void vidd_redraw_line(struct vidd_client* client)
{
	cursor_erase_line();
	struct line* line = client->cursor.y;

	cursor_move_to(vidd_view_get_absolute_x_offset(client), client->cursor.y->number - 1);
	printf("%.*s", (int)client->view.width, line->buffer.data + client->view.x);

	vidd_set_status(client);
	vidd_cursor_adjust(client);
}
void vidd_toggle_drawing(struct vidd_client* client)
{
	if (client->displayOn)
	{
		client->displayOn = false;
	}
	else
	{
		client->displayOn = true;
		vidd_redraw(client);
	}
}





void vidd_check_for_window_size_change(struct vidd_client* client)
{
	//screen_get_size(&client->width, &client->height);
	client->view.width = client->width;
	client->view.height = client->height - 1;
}





void vidd_set_status(struct vidd_client* client)
{
	char* fmt = INACTIVE_CLIENT_COLOR;
	if (client == vidd_get_active())
		fmt = ACTIVE_CLIENT_COLOR;
	cursor_save();

	vidd_line_clear(client, client->height - 1);

	cursor_move_to(client->x, client->y + client->height - 1);
	printf("%s%s" NOSTYLE, fmt, VIDD_MODE_TEXTS[client->mode]);

	cursor_move_to(client->x + client->width - strlen(client->file_name) - 1, client->y + client->height - 1);

	printf("%s%s" NOSTYLE, fmt, client->file_name);

	cursor_restore();
}





bool vidd_view_fix_overflow(struct vidd_client* client)
{
	bool redraw = false;

	int oov = 0;
	if ((oov = vidd_cursor_is_out_of_view(client)))
	{
		if (oov == 3 || oov == 1)
			vidd_view_center_x(client);
		if (oov == 3 || oov == 2)
			vidd_view_center_y(client);
		redraw = true;
	}

	return redraw;
}
bool vidd_cursor_fix_overflow(struct vidd_client* client)
{
	bool redraw = false;

	if (client->cursor.x >= client->cursor.y->buffer.length)
	{
		if (client->mode == VIDD_MODE_NORMAL && client->cursor.x >= client->cursor.y->buffer.length)
			client->cursor.x = client->cursor.y->buffer.length-1;
		else client->cursor.x = client->cursor.y->buffer.length;
	}
	if (client->cursor.x < 0)
		client->cursor.x = 0;

	if ((client->cursor.y->number-1) - client->view.y == -1)
	{
		client->view.y--;
		redraw = true;
	}
	else if ((client->cursor.y->number-1) - client->view.y == client->view.height)
	{
		client->view.y++;
		redraw = true;
	}
	if (client->cursor.x - client->view.x == -1)
	{
		client->view.x--;
		redraw = true;
	}
	else if (client->cursor.x - client->view.x == client->view.width)
	{
		client->view.x++;
		redraw = true;
	}

	return redraw;
}
bool vidd_selection_redraw_needed(struct vidd_client* client)
{
	bool redraw = false;

	if (client->mode == VIDD_MODE_LINE_SELECT && client->cursor.y->number - 1 != client->selection.y1)
	{
		redraw = true;
		intmax_t y0 = client->selection.y0;
		intmax_t y1 = client->selection.y1;
		if (y1 > y0) SWAP(y0, y1);
		if (!redraw) vidd_selection_draw(client);
		client->selection.y1 = client->cursor.y->number - 1;
	}
	if (client->mode == VIDD_MODE_SELECT && (client->cursor.y->number - 1 != client->selection.y1 ||
							client->cursor.x != client->selection.x1))
	{
		redraw = true;
		intmax_t y0 = client->selection.y0;
		intmax_t y1 = client->selection.y1;
		intmax_t x0 = client->selection.x0;
		intmax_t x1 = client->selection.x1;
		if (y1 > y0) SWAP(y0, y1);
		if (x1 > x0) SWAP(x0, x1);
		if (!redraw) vidd_selection_draw(client);
		client->selection.y1 = client->cursor.y->number - 1;
		client->selection.x1 = client->cursor.x;
	}

	return redraw;
}
void vidd_cursor_adjust(struct vidd_client* client)
{
	bool redraw = false;

	vidd_view_adjust_offset(client);
	if (vidd_cursor_fix_overflow(client) && !redraw) redraw = true;
	if (vidd_view_fix_overflow(client) && !redraw) redraw = true;
	if (vidd_selection_redraw_needed(client) && !redraw) redraw = true;

	if (redraw) vidd_redraw(client);
	cursor_move_to(vidd_view_get_absolute_x_offset(client) + client->cursor.x - client->view.x,
				vidd_view_get_absolute_y_offset(client) + (client->cursor.y->number - 1) - client->view.y);
}





void vidd_text_clear(struct vidd_client* client)
{
	struct line* line = client->text->next;
	while (line)
	{
		if (line->next)
		{
			line = line->next;
			free_line(line->prev);
		}
		else
		{
			free_line(line);
			break;
		}
	}
	line_set_text(client->text, "", 0);
	client->text->next = 0;
	client->text->prev = 0;
	client->text->number = 1;
}





intmax_t vidd_last_movable(struct vidd_client* client)
{
	if (client->mode == VIDD_MODE_NORMAL)
		return client->cursor.y->buffer.length-1;
	else return client->cursor.y->buffer.length;
}
int vidd_cursor_is_out_of_view(struct vidd_client* client)
{
	int oov = 0;
	if ((client->cursor.x < client->view.x) || ((client->cursor.x - client->view.x) >= client->view.width))
		oov += 1;
	if (((client->cursor.y->number - 1) < client->view.y) || (((client->cursor.y->number - 1) - client->view.y) >= client->view.height))
		oov += 2;
	return oov;
}
intmax_t vidd_view_get_absolute_x_offset(struct vidd_client* client)
{
	return client->x + client->view.xo;
}
intmax_t vidd_view_get_absolute_y_offset(struct vidd_client* client)
{
	return client->y + client->view.yo;
}





void vidd_cursor_center(struct vidd_client* client)
{
	vidd_move_to(client, client->cursor.x, (2 * client->view.y + client->view.height) / 2);
}





void vidd_move_up(struct vidd_client* client)
{
	if (client->cursor.y->prev)
		client->cursor.y = client->cursor.y->prev;
	client->cursor.x = client->cursor.lx;
	vidd_cursor_adjust(client);
}
void vidd_move_down(struct vidd_client* client)
{
	if (client->cursor.y->next)
		client->cursor.y = client->cursor.y->next;
	client->cursor.x = client->cursor.lx;
	vidd_cursor_adjust(client);
}
void vidd_move_right(struct vidd_client* client)
{
	client->cursor.x++;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}
void vidd_move_left(struct vidd_client* client)
{
	if (client->cursor.x == 0) return;
	client->cursor.x--;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}
void vidd_move_to_line_start(struct vidd_client* client)
{
	client->cursor.x = 0;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}
void vidd_move_to_line_end(struct vidd_client* client)
{
	client->cursor.x = vidd_last_movable(client);
	client->cursor.lx = 696969696969696969;
	vidd_cursor_adjust(client);
}
void vidd_move_next_word_end(struct vidd_client* client)
{
	if (client->cursor.x == (client->cursor.y->buffer.length-1))
	{
		vidd_move_down(client);
		vidd_move_to_line_start(client);
		vidd_move_next_word_end(client);
		return;
	}
	struct buffer text = client->cursor.y->buffer;
	int i = client->cursor.x;

	if (i < (text.length-1) && IS_CHARACTER(text.data[i]) && !IS_CHARACTER(text.data[i+1])) i++;
	else if (i < (text.length-1) && !IS_CHARACTER(text.data[i]) && IS_CHARACTER(text.data[i+1])) i++;

	if (text.data[i] == ' ')
		while (i <= (text.length-1) && text.data[i] == ' ') i++;

	if (IS_CHARACTER(text.data[i]))
		while ((i+1) <= (text.length-1) && IS_CHARACTER(text.data[i+1])) i++;
	else while ((i+1) <= (text.length-1) && !IS_CHARACTER(text.data[i+1]) && text.data[i+1] != ' ') i++;

	client->cursor.x = i;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}
void vidd_move_next_word(struct vidd_client* client)
{
	struct buffer text = client->cursor.y->buffer;
	int i = client->cursor.x;

#define VIDD_MOVE_NEXT_WORD_LINE_END_CONDITION() \
	{ \
		if (i == (text.length - 1)) \
		{ \
			vidd_move_down(client); \
			vidd_move_to_line_start(client); \
			return; \
		} \
		else i++; \
	}

	if (text.data[i] == ' ')
	{
		while (i <= (text.length-1) && text.data[i] == ' ')
			VIDD_MOVE_NEXT_WORD_LINE_END_CONDITION();
	}
	else
	{
		if (i < (text.length-1)) i++;

		if (text.data[i] == ' ')
		{
			while (i <= (text.length-1) && text.data[i] == ' ')
				VIDD_MOVE_NEXT_WORD_LINE_END_CONDITION();
		}
		else if (IS_CHARACTER(text.data[i]))
		{
			while (i <= (text.length-1) && IS_CHARACTER(text.data[i]))
				VIDD_MOVE_NEXT_WORD_LINE_END_CONDITION();
		}
		else if (!IS_CHARACTER(text.data[i]))
		{
			while (i <= (text.length-1) && !IS_CHARACTER(text.data[i]))
				VIDD_MOVE_NEXT_WORD_LINE_END_CONDITION();
		}

		if (text.data[i] == ' ')
		{
			while (i <= (text.length-1) && text.data[i] == ' ')
				VIDD_MOVE_NEXT_WORD_LINE_END_CONDITION();
		}
	}

	client->cursor.x = i;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}
void vidd_move_prev_word(struct vidd_client* client)
{
	if (client->cursor.x == 0)
	{
		vidd_move_up(client);
		vidd_move_to_line_end(client);
		vidd_move_prev_word(client);
		return;
	}
	struct buffer text = client->cursor.y->buffer;
	int i = client->cursor.x;

	if (text.data[i] == ' ') while (i > 0 && text.data[i] == ' ') i--;

	if (i > 0 && IS_CHARACTER(text.data[i]) && !IS_CHARACTER(text.data[i-1])) i--;
	else if (i > 0 && !IS_CHARACTER(text.data[i]) && IS_CHARACTER(text.data[i-1])) i--;

	if (text.data[i-1] == ' ') i--;
	if (text.data[i] == ' ') while (i > 0 && text.data[i] == ' ') i--;

	if (IS_CHARACTER(text.data[i]))
		while (i > 0 && (text.data[i-1] != ' ' && IS_CHARACTER(text.data[i-1]))) i--;
	else while (i > 0 && (text.data[i-1] != ' ' && !IS_CHARACTER(text.data[i-1]))) i--;

	client->cursor.x = i;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}
void vidd_move_to_spot_in_view_after(struct vidd_client* client, intmax_t pos)
{
	struct line* line = line_get_line(client->cursor.y, client->view.y+1);
	for (intmax_t i = 0; line && i < client->view.height; i++)
	{
		if (line->buffer.length > pos)
		{
			vidd_move_to(client, pos, line->number);
			return;
		}
		line = line->next;
	}
}





void vidd_view_adjust_offset(struct vidd_client* client)
{
	client->view.height = client->height - 1;
	intmax_t number_length = number_get_length(line_get_last(client->text)->number);
	if (client->view.xo != number_length + 1 ||
		client->view.width != client->width - number_length + 1)
	{
		client->view.xo = number_length + 1;
		client->view.width = client->width - client->view.xo;
	}
}





void vidd_view_center_y(struct vidd_client* client)
{
	client->view.y = (client->cursor.y->number - 1) - (client->view.height / 2);
	if (client->view.y < 0) client->view.y = 0;
}
void vidd_view_center_x(struct vidd_client* client)
{
	client->view.x = (client->cursor.x - 1) - (client->view.width / 2);
	if (client->view.x < 0) client->view.x = 0;
}
void vidd_view_center(struct vidd_client* client)
{
	vidd_view_center_x(client);
	vidd_view_center_y(client);
	vidd_redraw(client);
}





void vidd_view_move_right(struct vidd_client* client)
{
	client->view.x++;
	if (vidd_cursor_is_out_of_view(client))
	{
		if (client->cursor.x == client->view.x-1 && client->cursor.x == client->cursor.y->buffer.length-1)
		{
			vidd_move_to_spot_in_view_after(client, client->cursor.x + 1);
		}
		else vidd_move_right(client);
	}
	vidd_redraw(client);
}
void vidd_view_move_left(struct vidd_client* client)
{
	if (client->view.x == 0) return;
	client->view.x--;
	if (vidd_cursor_is_out_of_view(client))
	{
		vidd_move_left(client);
	}
	vidd_redraw(client);
}
void vidd_view_move_up(struct vidd_client* client)
{
	client->view.y--;
	if (vidd_cursor_is_out_of_view(client))
	{
		vidd_move_up(client);
	}
	vidd_redraw(client);
}
void vidd_view_move_down(struct vidd_client* client)
{
	client->view.y++;
	if (vidd_cursor_is_out_of_view(client))
	{
		vidd_move_down(client);
		client->view.y++;
	}
	vidd_redraw(client);
}





void vidd_set_marker(struct vidd_client* client)
{
	uint32_t c = getch();
	client->marker.positions[c].x = client->cursor.x;
	client->marker.positions[c].y = client->cursor.y->number;
}
void vidd_goto_marker(struct vidd_client* client)
{
	uint32_t c = getch();
	vidd_move_to(client, client->marker.positions[c].x, client->marker.positions[c].y);
}





void vidd_goto_top(struct vidd_client* client)
{
	vidd_move_to(client, 0, 0);
}
void vidd_goto_bottom(struct vidd_client* client)
{
	vidd_move_to(client, 0, line_get_last(client->cursor.y)->number);
}
void vidd_move_to(struct vidd_client* client, intmax_t x, intmax_t y)
{
	client->cursor.x = x;
	client->cursor.y = line_get_line(client->cursor.y, y);
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}





void vidd_frame_move_right(struct vidd_client* client)
{
	client->x++;
	vidd_redraw(client);
}





void vidd_new_line(struct vidd_client* client)
{
	line_insert(client->cursor.y);
	vidd_move_down(client);
	vidd_redraw(client);
}
void vidd_insert_on_new_line(struct vidd_client* client)
{
	line_insert(client->cursor.y);
	vidd_move_down(client);
	vidd_redraw(client);
	vidd_enter_insert_mode(client);
}
void vidd_insert_on_new_line_up(struct vidd_client* client)
{
	if (client->cursor.y->number - 1 == 0)
	{
		vidd_move_to_line_start(client);
		vidd_split_at_cursor(client);
		vidd_move_up(client);
		vidd_enter_insert_mode(client);
	}
	else
	{
		vidd_move_up(client);
		vidd_insert_on_new_line(client);
	}
}





void vidd_indent(struct vidd_client* client)
{
	line_insert_at(client->cursor.y, ' ', 0);
	line_insert_at(client->cursor.y, ' ', 0);
	line_insert_at(client->cursor.y, ' ', 0);
	line_insert_at(client->cursor.y, ' ', 0);

	vidd_move_right(client);
	vidd_move_right(client);
	vidd_move_right(client);
	vidd_move_right(client);

	vidd_redraw(client);
}
void vidd_deindent(struct vidd_client* client)
{
	intmax_t sspn = strspn(client->cursor.y->buffer.data, " ");
	if (sspn < 4) return;

	line_delete_at(client->cursor.y, 0);
	line_delete_at(client->cursor.y, 0);
	line_delete_at(client->cursor.y, 0);
	line_delete_at(client->cursor.y, 0);

	vidd_move_left(client);
	vidd_move_left(client);
	vidd_move_left(client);
	vidd_move_left(client);

	vidd_redraw(client);
}




void vidd_split_at_cursor(struct vidd_client* client)
{
	line_split_at(client->cursor.y, client->cursor.x);
	vidd_move_down(client);
	vidd_move_to_line_start(client);
	vidd_redraw(client);
}





void vidd_delete(struct vidd_client* client)
{
	if (client->cursor.x == client->cursor.y->buffer.length)
	{
		line_join_next(client->cursor.y);
		vidd_redraw(client);
		return;
	}
	intmax_t sspn = strspn(client->cursor.y->buffer.data, " ");
	if (client->cursor.x <= sspn && sspn % 4 == 0 && (client->cursor.x+1) % 4 == 0)
	{
		line_delete_at(client->cursor.y, client->cursor.x); vidd_move_left(client);
		line_delete_at(client->cursor.y, client->cursor.x); vidd_move_left(client);
		line_delete_at(client->cursor.y, client->cursor.x); vidd_move_left(client);
		line_delete_at(client->cursor.y, client->cursor.x);
	}
	else line_delete_at(client->cursor.y, client->cursor.x);
	vidd_redraw(client);
}
void vidd_backspace(struct vidd_client* client)
{
	if (client->cursor.x == 0 && client->cursor.y->number != 1)
	{
		vidd_move_up(client);
		vidd_move_to_line_end(client);
		line_join_next(client->cursor.y);
		vidd_redraw(client);
		return;
	}
	vidd_move_left(client);
	vidd_delete(client);
}
void vidd_delete_movement(struct vidd_client* client, char key)
{
	intmax_t ox = client->cursor.x;
	intmax_t oy = client->cursor.y->number-1;
	client->mode = VIDD_MODE_SELECT;
	vidd_interrupt(client, key);
	client->selection.x0 = ox;
	client->selection.y0 = oy;
	client->selection.x1 = client->cursor.x;
	client->selection.y1 = client->cursor.y->number-1;
	vidd_delete_selection(client);
	client->mode = VIDD_MODE_NORMAL;
}
void vidd_delete_selection(struct vidd_client* client)
{
	struct line* line;
	if (client->mode == VIDD_MODE_LINE_SELECT)
	{
		VIDD_LINE_SELECTION_LOOP({
			line = line_remove(line);
		});
		client->cursor.y = line;
	}
	else
	{
		VIDD_SELECTION_LOOP_CRSMEJ(
		{}, {//range
			line_delete_n_at(line, x1-x0, x0);
		}, {//start
			line_delete_after(line, x0);
		}, {//middle
			line_remove(line);
		}, {//end
			line_delete_before(line, x1);
		}, {//join
			line_join_next(first_line);
		});
		intmax_t ex0 = client->selection.x0;
		intmax_t ex1 = client->selection.x1;
		intmax_t ey0 = client->selection.y0;
		intmax_t ey1 = client->selection.y1;

		if (client->selection.y0 == client->selection.y1)
			{ if (ex0 > ex1) SWAP(ex0, ex1); }
		else if (client->selection.y0 > client->selection.y1)
			SWAP(ey0, ey1);

		vidd_move_to(client, ex0, ey0+1);
	}

	vidd_enter_normal_mode(client);
	vidd_redraw(client);
}
void vidd_delete_line(struct vidd_client* client)
{
	client->cursor.y = line_remove(client->cursor.y);
	if (client->cursor.y->number == 1) client->text = client->cursor.y;
	vidd_redraw(client);
}
void vidd_delete_two_lines_up(struct vidd_client* client)
{
	vidd_move_up(client);
	vidd_delete_line(client);
	vidd_delete_line(client);
}
void vidd_delete_two_lines_down(struct vidd_client* client)
{
	vidd_delete_line(client);
	vidd_delete_line(client);
}





void vidd_replace_character(struct vidd_client* client)
{
	char key = getch();
	client->cursor.y->buffer.data[client->cursor.x] = key;
	vidd_redraw(client);
}





void vidd_selection_draw(struct vidd_client* client)
{
	struct line* line;
	cursor_save();
	if (client->mode == VIDD_MODE_LINE_SELECT)
	{
		VIDD_LINE_SELECTION_LOOP({
			cursor_move_to(vidd_view_get_absolute_x_offset(client),
					vidd_view_get_absolute_y_offset(client) + i - client->view.y);
			printf(STYLE_UNDERLINE);
			if (client->view.x < line->buffer.length)
				printf("%.*s", (int)client->view.width, &line->buffer.data[client->view.x]);
			printf(NOSTYLE);
			line = line->next;
		});
	}
	else
	{
		VIDD_SELECTION_LOOP_CRSMEJ({
			cursor_move_to(vidd_view_get_absolute_x_offset(client),
					vidd_view_get_absolute_y_offset(client) + i - client->view.y);
		}, {// range 
			cursor_move(x0, 0);
			printf(STYLE_UNDERLINE "%.*s" NOSTYLE, (int)MIN(client->view.width, x1-x0), &line->buffer.data[x0]);
		}, {// start
			cursor_move(x0, 0);
			printf(STYLE_UNDERLINE "%.*s" NOSTYLE, (int)MIN(client->view.width, line->buffer.length-x0+1), &line->buffer.data[x0]);
		}, {// middle
			printf(STYLE_UNDERLINE "%.*s" NOSTYLE, (int)MIN(client->view.width, line->buffer.length), line->buffer.data);
		}, {// end
			printf(STYLE_UNDERLINE "%.*s" NOSTYLE, (int)MIN(client->view.width, x1), line->buffer.data);
		}, {});
	}
	cursor_restore();
}
void vidd_line_selection_draw(struct vidd_client* client)
{
	intmax_t y0 = client->selection.y0;
	intmax_t y1 = client->cursor.y->number - 1;
	int dy = 1;

	if (y0 > y1)
	{
		dy = -1;
	}

	intmax_t line_start = MAX(y0, client->view.y);

	struct line* line = line_get_line(client->cursor.y, line_start+1);

	cursor_save();

	intmax_t p = line_start;
	for (intmax_t i = line_start; i != y1 + dy; i += dy)
	{
		cursor_move_to(vidd_view_get_absolute_x_offset(client),
				vidd_view_get_absolute_y_offset(client) + p - client->view.y);
		printf(STYLE_UNDERLINE);
		if (client->view.x < line->buffer.length)
			printf("%.*s", (int)client->view.width, &line->buffer.data[client->view.x]);
		printf(NOSTYLE);
		line = line_get_line_in_direction(line, dy);
		p += dy;
	}
	cursor_restore();
	
}





void vidd_copy_line(struct vidd_client* client)
{
	buffer_clear(&copy_buffer);
	buffer_push(&copy_buffer, 27);
	buffer_push(&copy_buffer, 255);
	buffer_push(&copy_buffer, 'o');
	buffer_push_cstring(&copy_buffer, client->cursor.y->buffer.data, client->cursor.y->buffer.length);
	buffer_push(&copy_buffer, 27);
	buffer_push(&copy_buffer, '^');
	buffer_push(&copy_buffer, 255);
}
void vidd_selection_copy(struct vidd_client* client)
{
	struct line* line;

	buffer_clear(&copy_buffer);
	buffer_push(&copy_buffer, 27);
	buffer_push(&copy_buffer, 255);
	if (client->mode == VIDD_MODE_LINE_SELECT)
	{
		buffer_push(&copy_buffer, 'o');

		VIDD_LINE_SELECTION_LOOP({
			buffer_push_cstring(&copy_buffer, line->buffer.data, line->buffer.length);
			if (i + 1 <= lim) buffer_push(&copy_buffer, '\n');
			line = line->next;
		});
		buffer_push(&copy_buffer, 27);
	}
	else
	{
		buffer_push(&copy_buffer, 'a');

		VIDD_SELECTION_LOOP_CRSMEJ(
		{}, {//range
			buffer_push_cstring(&copy_buffer, &line->buffer.data[x0], x1-x0);
		}, {//start
			buffer_push_cstring(&copy_buffer, &line->buffer.data[x0], line->buffer.length-x0);
			buffer_push(&copy_buffer, '\n');
		}, {//middle
			buffer_push_cstring(&copy_buffer, line->buffer.data, line->buffer.length);
			buffer_push(&copy_buffer, '\n');
		}, {//end
			buffer_push_cstring(&copy_buffer, line->buffer.data, x1);
		}, {});
		buffer_push(&copy_buffer, 27);
	}
	buffer_push(&copy_buffer, 255);
}





void vidd_paste(struct vidd_client* client)
{
	buffer_copy_inverse(&run_buffer, &copy_buffer);
}





void vidd_find_next_word(struct vidd_client* client, char* word, intmax_t length)
{
	if (!word) return;
	struct line* line = client->cursor.y;
	char* text = &line->buffer.data[client->cursor.x+1];
	while (line)
	{
		char* lct = strstr(text, word);
		if (lct)
		{
			vidd_move_to(client, (intmax_t)(lct - text), line->number);
			return;
		}
		line = line->next;
		if (line) text = line->buffer.data;
	}
}





void vidd_enter_normal_mode(struct vidd_client* client)
{
	if (client->mode == VIDD_MODE_LINE_SELECT || client->mode == VIDD_MODE_SELECT)
	{
		client->mode = VIDD_MODE_NORMAL;
		vidd_redraw(client);
	}
	client->mode = VIDD_MODE_NORMAL;
	vidd_set_status(client);
}
void vidd_enter_insert_mode(struct vidd_client* client)
{
	client->mode = VIDD_MODE_INSERT;
	vidd_set_status(client);
}
void vidd_enter_insert_mode_right(struct vidd_client* client)
{
	client->mode = VIDD_MODE_INSERT;
	vidd_move_right(client);
}
void vidd_enter_replace_mode(struct vidd_client* client)
{
	client->mode = VIDD_MODE_REPLACE;
	vidd_set_status(client);
}
void vidd_enter_command_mode(struct vidd_client* client)
{
	cursor_save();
	cursor_move_to(client->x + strlen(VIDD_MODE_TEXTS[client->mode]), client->y + client->height - 1);
	printf(ACTIVE_CLIENT_COLOR);
	printf(":");
	client->mode = VIDD_MODE_COMMAND;
}
void vidd_enter_find_next_mode(struct vidd_client* client)
{
	cursor_save();
	cursor_move_to(client->x + strlen(VIDD_MODE_TEXTS[client->mode]), client->y + client->height);
	printf(ACTIVE_CLIENT_COLOR);
	printf("/");
	client->mode = VIDD_MODE_FIND;
}
void vidd_enter_find_prev_mode(struct vidd_client* client)
{
	cursor_save();
	cursor_move_to(client->x + strlen(VIDD_MODE_TEXTS[client->mode]), client->y + client->height);
	printf(ACTIVE_CLIENT_COLOR);
	printf("?");
	client->mode = VIDD_MODE_FIND;
}
void vidd_enter_select_mode(struct vidd_client* client)
{
	client->selection.x0 = client->cursor.x;
	client->selection.x1 = client->cursor.x;
	client->selection.y0 = client->cursor.y->number - 1;
	client->selection.y1 = client->cursor.y->number - 1;
	client->mode = VIDD_MODE_SELECT;
	vidd_set_status(client);
	vidd_selection_draw(client);
}
void vidd_enter_line_select_mode(struct vidd_client* client)
{
	client->selection.x0 = 0;
	client->selection.y0 = client->cursor.y->number-1;
	client->selection.y1 = client->cursor.y->number - 1;

	client->mode = VIDD_MODE_LINE_SELECT;
	vidd_set_status(client);
	vidd_selection_draw(client);
}





void vidd_client_up(struct vidd_client* client)
{
}
void vidd_client_down(struct vidd_client* client)
{
}
void vidd_client_prev(struct vidd_client* client)
{
	vidd_client_pool_prev_client(&client_pool);
	vidd_redraw(client);
	vidd_redraw(vidd_get_active());
}
void vidd_client_next(struct vidd_client* client)
{
	vidd_client_pool_next_client(&client_pool);
	vidd_redraw(client);
	vidd_redraw(vidd_get_active());
}

void vidd_swap_active(void)
{
}
void vidd_draw_vsplit_line(struct vidd_client* client)
{
	cursor_move_to(client->x-1, client->y);
	for (int i = 0; i < client->height; i++)
	{
		printf(BRGB("255", "255", "255") FRGB("0", "0", "0") "|" NOSTYLE);
		if (i + 1 < client->height) printf(CURSOR_DOWN() CURSOR_LEFT());
	}
}





void vidd_open_empty(struct vidd_client* client)
{
	vidd_vsplit(client, "_-=[NONE]=-_");
}





void vidd_edit(struct vidd_client* client, char* args)
{
	char* file_name = client->file_name;
	if (strlen(args) > 0)
		file_name = args;

	struct vidd_client tmp = *client;
	free_vidd_client(client);
	*client = make_vidd_client(file_name, tmp.x, tmp.y, tmp.width, tmp.height);
	vidd_load_file(client, file_name);
	client->view.xo = tmp.view.xo;
	client->view.yo = tmp.view.yo;
	client->view.height = tmp.view.height;
	client->view.width = tmp.view.width;
	client->width = tmp.width;
	client->height = tmp.height;
	client->x = tmp.x;
	client->y = tmp.y;
	vidd_redraw(client);
}
void vidd_split(struct vidd_client* client, char* args)
{
}
void vidd_client_reorganize(struct vidd_client_pool* pool)
{
	struct vidd_client* master = &pool->clients[0];
	screen_get_size(&master->width, &master->height);
	master->x = 0;
	master->y = 0;

	if (pool->length == 1)
	{
		vidd_redraw(master);
		return;
	}

	master->width /= 2;

	vidd_redraw(master);

	struct vidd_client* prev = &pool->clients[1];

	prev->x = master->width + 1;
	prev->y = 0;
	prev->width = master->width;
	prev->height = master->height/(pool->length-1);

	vidd_redraw(prev);

	vidd_draw_vsplit_line(prev);

	if (pool->length == 2) return;

	for (intmax_t i = 2; i < pool->length; i++)
	{
		struct vidd_client* cur = &pool->clients[i];
		cur->x = prev->x;
		cur->y = prev->y + prev->height;
		cur->width = prev->width;
		cur->height = prev->height;
		vidd_redraw(cur);
		prev = cur;
	}
	if ((master->height/(pool->length-1)) * (pool->length-1) != master->height)
	{
		prev->height += master->height - ((master->height/(pool->length-1)) * (pool->length-1));
		vidd_redraw(prev);
	}
}
void vidd_swap(struct vidd_client* client)
{
	if (client == &client_pool.clients[0])
	{
		if (client_pool.length == 1) return;
		client = &client_pool.clients[1];
	}
	struct vidd_client tmp = client_pool.clients[0];
	client_pool.clients[0] = *client;
	*client = tmp;

	client_pool.active = 0;

	vidd_client_reorganize(&client_pool);
}
void vidd_vsplit(struct vidd_client* client, char* args)
{
	char* file_name = args;
	if (strlen(args) == 0)
		file_name = client->file_name;
	
	struct vidd_client new_client = make_vidd_client(file_name, 0, 0, 0, 0);

	struct vidd_client* new_client_ptr = vidd_client_pool_add(&client_pool, new_client);

	vidd_load_file(new_client_ptr, file_name);

	vidd_client_reorganize(&client_pool);
}
void vidd_run_command_in_frame(struct vidd_client* client, char* args)
{
	intmax_t comlen = strlen(args) + 6;
	char* com = malloc(comlen);
	memcpy(com, args, comlen-6);
	memcpy(com + comlen-6, " 2>&1", 5);
	com[comlen] = 0;

	intmax_t borderw = client->width / 23;
	intmax_t borderh = client->height / 10;

	struct frame frame = { client->x + borderw, client->y + borderh, client->view.width - (2 * borderw), client->view.height - (2 * borderh), { 0 }, 0, 0 };
	frame.buffer = make_buffer(150);
	char line[2048] = {0};
	FILE* fp = popen(com, "r");
	while (fgets(line, sizeof(line), fp))
	{
		buffer_push_cstring(&frame.buffer, line, strlen(line));
	}
	frame_draw(&frame);
	free_buffer(&frame.buffer);
	free(com);
}
void vidd_write(struct vidd_client* client, char* args)
{
	char* file_name = client->file_name;
	if (strlen(args) > 0)
		file_name = args;

	if (!strcmp(file_name, "_-=[NONE]=-_")) return;

	FILE* fp = fopen(file_name, "w");
	if (fp == 0)
	{
		return;
	}

	struct line* line = client->text;
	while (line)
	{
		bool nontabs_found = false;
		for (intmax_t i = 0; i < line->buffer.length; i++)
		{
	    		if (!nontabs_found && *(uint32_t*)&line->buffer.data[i] == *(uint32_t*)"    ")
			{
				fputc('\t', fp);
				i += 3;
			}
			else
			{
				fputc(line->buffer.data[i], fp);
				nontabs_found = true;
			}
		}
		if (line->next) fwrite("\n", 1, 1, fp);
		line = line->next;
	}
	fclose(fp);
}
void vidd_client_pool_remove(struct vidd_client_pool* pool, struct vidd_client* client)
{
	intmax_t pos = vidd_client_pool_get_client_index(pool, client);
//	free_vidd_client(client);
	for (intmax_t i = pos; i < pool->length - 1; i++)
		pool->clients[i] = pool->clients[i+1];
	pool->length--;
	pool->clients[pool->length] = (struct vidd_client){ 0 };
	pool->active = 0;
}
void vidd_write_quit(struct vidd_client* client, char* args)
{
	vidd_write(client, args);
	getch_exit();
	screen_restore();
	exit(0);
}

void vidd_client_quit(struct vidd_client* client, char* args)
{
	if (client_pool.length == 1) vidd_exit(client, 0);

	vidd_client_pool_remove(&client_pool, client);

	vidd_client_pool_next_client(&client_pool);
	
	vidd_client_reorganize(&client_pool);
}
void vidd_exit(struct vidd_client* client, char* args)
{
	getch_exit();
	screen_restore();
	exit(0);
}





void vidd_void(struct vidd_client* client) {}

#endif