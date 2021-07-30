#ifndef __VIDD_COMMANDS_H__
#define __VIDD_COMMANDS_H__

#include "./syntax.h"

void vidd_redraw(struct vidd_client* client);
void vidd_redraw_line(struct vidd_client* client);
void vidd_toggle_drawing(struct vidd_client* client);
void vidd_reorganize_clients(struct vidd_client_pool* pool);
void vidd_increase_master_size(struct vidd_client* client);
void vidd_decrease_master_size(struct vidd_client* client);

void vidd_floating_window_draw_frame(struct vidd_client* client);
void vidd_floating_increase_height(struct vidd_client* client);
void vidd_floating_decrease_height(struct vidd_client* client);
void vidd_floating_increase_width(struct vidd_client* client);
void vidd_floating_decrease_width(struct vidd_client* client);
void vidd_move_floating_up(struct vidd_client* client);
void vidd_move_floating_down(struct vidd_client* client);
void vidd_move_floating_right(struct vidd_client* client);
void vidd_move_floating_left(struct vidd_client* client);

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
void vidd_move_to_x(struct vidd_client* client, intmax_t x);

void vidd_frame_move_right(struct vidd_client* client);

void vidd_insert_char(struct vidd_client* client);

void vidd_new_line(struct vidd_client* client);
void vidd_insert_on_new_line(struct vidd_client* client);
void vidd_insert_on_new_line_up(struct vidd_client* client);

void vidd_indent(struct vidd_client* client);
void vidd_deindent(struct vidd_client* client);

void vidd_split_at_cursor(struct vidd_client* client);

void vidd_delete(struct vidd_client* client);
void vidd_delete_word(struct vidd_client* client);
void vidd_delete_movement(struct vidd_client* client, char key);
void vidd_delete_selection(struct vidd_client* client);
void vidd_delete_line(struct vidd_client* client);
void vidd_backspace(struct vidd_client* client);
void vidd_delete_two_lines_up(struct vidd_client* client);
void vidd_delete_two_lines_down(struct vidd_client* client);

void vidd_replace_character(struct vidd_client* client);

void vidd_line_selection_draw(struct vidd_client* client);
void vidd_selection_draw(struct vidd_client* client);
void vidd_selection_swap_cursor(struct vidd_client* client);

void vidd_copy_line(struct vidd_client* client);
void vidd_selection_copy(struct vidd_client* client);

void vidd_paste(struct vidd_client* client);

void vidd_find_next_char(struct vidd_client* client);
void vidd_find_prev_char(struct vidd_client* client);
void vidd_find_next_word(struct vidd_client* client, char* word, intmax_t length);
void vidd_find_prev_word(struct vidd_client* client, char* word, intmax_t length);
void vidd_find_next_word_under_cursor(struct vidd_client* client);
void vidd_find_prev_word_under_cursor(struct vidd_client* client);
void vidd_repeat_last_find(struct vidd_client* client);
void vidd_repeat_last_find_reverse(struct vidd_client* client);

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
void vidd_run_command_in_vsplit(struct vidd_client* client, char* args);
void vidd_run_command_in_floating_window(struct vidd_client* client, char* args);
void vidd_open_in_floating_window(struct vidd_client* client, char* args);
void vidd_split(struct vidd_client* client, char* args);
void vidd_duplicate(struct vidd_client* client);
void vidd_vsplit(struct vidd_client* client, char* args);
void vidd_write(struct vidd_client* client, char* args);
void vidd_write_quit(struct vidd_client* client, char* args);
void vidd_quit_current(struct vidd_client* client);
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

	vidd_cursor_adjust(client);

	vidd_view_adjust_offset(client);

	struct line* line = client->text;
	intmax_t line_count = line_get_last(line)->number;
	intmax_t line_number_gap = number_get_length(line_count);
	intmax_t visable_line_count = client->view.height;

	struct buffer toprint = make_buffer(client->view.height * client->view.width * 4);

	if (client->view.y < 0)
	{
		buffer_printf(&toprint, CURSOR_TO("%d", "%d") STYLE_EMPTY_LINE, client->y - client->view.y + 1 - 1, client->x+1);
		buffer_push_repeat(&toprint, ' ', client->width-1);
		buffer_print(&toprint, CURSOR_DOWN());
		buffer_printf(&toprint, CURSOR_TO("%d", "%d"), client->y - client->view.y + 1, client->x+1);
		visable_line_count += client->view.y;
	}
	else
	{
		line = line_skip(line, client->view.y);
		buffer_printf(&toprint, CURSOR_TO("1", "%d"), vidd_view_get_absolute_y_offset(client)+1);
	}

	if (client->x != 0 && client->view.y >= 0)
		buffer_printf(&toprint, CURSOR_RIGHT("%d"), client->x);

	intmax_t i;
	for (i = 0; line && i < visable_line_count; i++)
	{
		buffer_push_repeat(&toprint, ' ', line_number_gap - number_get_length(line->number));

		const intmax_t line_style_length = sizeof(STYLE_LINE_NUMBER) - 1 - 2;

		buffer_printf(&toprint, STYLE_LINE_NUMBER_COLOR STYLE_LINE_NUMBER NOSTYLE, line->number);

		if (client->view.xo > line_number_gap + line_style_length)
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
		buffer_print(&toprint, FRGB("255", "255", "0"));
		buffer_push(&toprint, '*');
		buffer_push_repeat(&toprint, ' ', client->view.width - 1);
		if (i + 1 < visable_line_count)
			buffer_printf(&toprint, ((client->x != 0) ? ("\r" CURSOR_DOWN() CURSOR_RIGHT("%d")) : ("\r" CURSOR_DOWN())), client->x);
		buffer_print(&toprint, NOSTYLE);
	}

	if (client->isFloating) vidd_floating_window_draw_frame(client);
	printf(NOSTYLE CURSOR_HIDE "%s" CURSOR_SHOW, toprint.data);

	free_buffer(&toprint);

	vidd_set_status(client);

	if (client->mode == VIDD_MODE_LINE_SELECT) vidd_selection_draw(client);
	if (client->mode == VIDD_MODE_SELECT) vidd_selection_draw(client);
	cursor_move_to(vidd_view_get_absolute_x_offset(client) + client->cursor.x - client->view.x,
				vidd_view_get_absolute_y_offset(client) + (client->cursor.y->number - 1) - client->view.y);
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
	intmax_t new_width, new_height;
	screen_get_size(&new_width, &new_height);
	if (new_width != client->width || new_height != client->height)
		vidd_reorganize_clients(&client_pool);
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

	cursor_move_to(client->x + client->width - strlen(client->file_name.data) - 1, client->y + client->height - 1);

	printf("%s%s" NOSTYLE, fmt, client->file_name.data);

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
		{
			client->cursor.x = client->cursor.y->buffer.length-1;
		}
		else
		{
			client->cursor.x = client->cursor.y->buffer.length;
		}
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
	if (client->cursor.x == vidd_last_movable(client))
	{
		printf("\a");
		return;
	}
	client->cursor.x++;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
}
void vidd_move_left(struct vidd_client* client)
{
	if (client->cursor.x == 0)
	{
		printf("\a");
		return;
	}
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

	if (i > 0 && IS_CHARACTER(text.data[i]) && !IS_CHARACTER(text.data[i-1])) i--;
	else if (i > 0 && text.data[i] != ' ' && !IS_CHARACTER(text.data[i]) && IS_CHARACTER(text.data[i-1])) i--;

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
	const intmax_t line_style_length = sizeof(STYLE_LINE_NUMBER) - 1 - 4;
	client->view.height = client->height - 1;
	intmax_t number_length = number_get_length(line_get_last(client->text)->number);
	if (client->view.xo != number_length + line_style_length ||
		client->view.width != client->width - number_length + line_style_length)
	{
		client->view.xo = number_length + line_style_length;
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
void vidd_move_to_x(struct vidd_client* client, intmax_t x)
{
	client->cursor.x = x;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client);
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
void vidd_delete_word(struct vidd_client* client)
{
	vidd_toggle_drawing(client);
	vidd_move_right(client);
	vidd_move_prev_word(client);
	vidd_enter_select_mode(client);
	vidd_move_next_word_end(client);
	vidd_move_right(client);
	vidd_delete_selection(client);
	vidd_toggle_drawing(client);
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
	client->displayOn = false;
	intmax_t ox = client->cursor.x;
	intmax_t oy = client->cursor.y->number-1;
	client->mode = VIDD_MODE_SELECT;
	vidd_interrupt(client, key);
	client->selection.x0 = ox;
	client->selection.y0 = oy;
	client->selection.x1 = client->cursor.x;
	client->selection.y1 = client->cursor.y->number-1;
	vidd_delete_selection(client);
	client->displayOn = true;
	client->mode = VIDD_MODE_NORMAL;
	vidd_redraw(client);
}
void vidd_delete_selection(struct vidd_client* client)
{
	struct line* line;
	if (client->mode == VIDD_MODE_LINE_SELECT)
	{
		VIDD_LINE_SELECTION_FULL_LOOP({
			line = line_remove(line);
		});
		client->cursor.y = line;
	}
	else
	{
		VIDD_SELECTION_FULL_LOOP_CRSMEJ(
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
	if (!client->displayOn) return;

	struct buffer toprint = make_buffer(ABS(client->selection.y1 - client->selection.y0)  * client->view.width);
	struct line* line;
	cursor_save();
	if (client->mode == VIDD_MODE_LINE_SELECT)
	{
		VIDD_LINE_SELECTION_LOOP({
			buffer_printf(&toprint, CURSOR_TO("%d", "%d"), vidd_view_get_absolute_y_offset(client) + i - client->view.y + 1, vidd_view_get_absolute_x_offset(client) + 1);
			buffer_printf(&toprint, STYLE_HIGHLIGHT);
			if (client->view.x < line->buffer.length)
				buffer_printf(&toprint, "%.*s", (int)client->view.width, &line->buffer.data[client->view.x]);
			if (line->buffer.length == 0)
				buffer_printf(&toprint, BRGB("255", "255", "255") " ");
			buffer_printf(&toprint, NOSTYLE);
			line = line->next;
		});
	}
	else
	{
		VIDD_SELECTION_LOOP_CRSMEJ({
			buffer_printf(&toprint, CURSOR_TO("%d", "%d"),
						vidd_view_get_absolute_y_offset(client) + i - client->view.y + 1,
						vidd_view_get_absolute_x_offset(client) + 1);
		}, {// range 
			if (x0 != 0)
				buffer_printf(&toprint, CURSOR_RIGHT("%d"), x0);
			buffer_printf(&toprint, STYLE_HIGHLIGHT "%.*s" NOSTYLE, (int)MIN(client->view.width, x1-x0), &line->buffer.data[x0]);
		}, {// start
			if (x0 != 0)
				buffer_printf(&toprint, CURSOR_RIGHT("%d"), x0);
			buffer_printf(&toprint, STYLE_HIGHLIGHT "%.*s" NOSTYLE, (int)MIN(client->view.width, line->buffer.length-x0+1), &line->buffer.data[x0]);
		}, {// middle
			buffer_printf(&toprint, STYLE_HIGHLIGHT "%.*s" NOSTYLE, (int)MIN(client->view.width, line->buffer.length), line->buffer.data);
			if (line->buffer.length == 0)
				buffer_printf(&toprint, BRGB("255", "255", "255") " ");
		}, {// end
			buffer_printf(&toprint, STYLE_HIGHLIGHT "%.*s" NOSTYLE, (int)MIN(client->view.width, x1), line->buffer.data);
			if (line->buffer.length == 0)
				buffer_printf(&toprint, BRGB("255", "255", "255") " ");
		}, {});
	}
	printf("%s", toprint.data);
	cursor_restore();
	free_buffer(&toprint);
}
void vidd_selection_swap_cursor(struct vidd_client* client)
{
	intmax_t x0 = client->selection.x0;
	intmax_t x1 = client->selection.x1;
	intmax_t y0 = client->selection.y0;
	intmax_t y1 = client->selection.y1;

	client->selection.x0 = x1;
	client->selection.y0 = y1;

	client->selection.x1 = x0;
	client->selection.y1 = y0;

	client->cursor.x = x0;
	client->cursor.y = line_get_line(client->cursor.y, y0+1);

	vidd_cursor_adjust(client);
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
		printf(STYLE_HIGHLIGHT);
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
			if (i + 1 <= lim) buffer_push(&copy_buffer, 13);
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
			buffer_push(&copy_buffer, 13);
		}, {//middle
			buffer_push_cstring(&copy_buffer, line->buffer.data, line->buffer.length);
			buffer_push(&copy_buffer, 13);
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





void vidd_find_next_char(struct vidd_client* client)
{
	unsigned char to_find = getch();
	intmax_t new_x = client->cursor.x+1;
	for (intmax_t i = new_x; i < client->cursor.y->buffer.length; i++)
	{
		if (client->cursor.y->buffer.data[i] == to_find)
		{
			new_x = i;
			break;
		}
	}
	vidd_move_to_x(client, new_x);
}
void vidd_find_prev_char(struct vidd_client* client)
{
	unsigned char to_find = getch();
	intmax_t new_x = client->cursor.x-1;
	for (intmax_t i = new_x; i >= 0; i--)
	{
		if (client->cursor.y->buffer.data[i] == to_find)
		{
			new_x = i;
			break;
		}
	}
	vidd_move_to_x(client, new_x);
}
void vidd_repeat_last_find(struct vidd_client* client)
{
	vidd_find_next_word(client, client->last_find.data, client->last_find.length);
}
void vidd_repeat_last_find_reverse(struct vidd_client* client)
{
	vidd_find_prev_word(client, client->last_find.data, client->last_find.length);
}
void vidd_find_next_word(struct vidd_client* client, char* word, intmax_t length)
{
	if (!word) return;
	struct line* line = client->cursor.y;
	intmax_t rx = client->cursor.x+1;
	char* text = &line->buffer.data[rx];
	while (line)
	{
		char* lct = strstr(text, word);
		if (lct)
		{
			vidd_move_to(client, rx + (intmax_t)(lct - text), line->number);
			return;
		}
		line = line->next;
		if (line)
		{
			text = line->buffer.data;
			rx = 0;
		}
	}
}
void vidd_find_prev_word(struct vidd_client* client, char* word, intmax_t length)
{
	if (!word) return;
	struct line* line = client->cursor.y;
	intmax_t rx = client->cursor.x;
	char* text = line->buffer.data;
	while (line)
	{
		char* lct = strlstr(text, word, rx);
		if (lct)
		{
			vidd_move_to(client, (intmax_t)(lct - text), line->number);
			return;
		}
		line = line->prev;
		if (line)
		{
			rx = line->buffer.length;
			text = line->buffer.data;
		}
	}
}
void vidd_find_next_word_under_cursor(struct vidd_client* client)
{
	if (!IS_CHARACTER(client->cursor.y->buffer.data[client->cursor.x])) return;
	intmax_t i = client->cursor.x;
	while (i > 0 && IS_CHARACTER(client->cursor.y->buffer.data[i])) i--;
	intmax_t start = i + 1;

	i = client->cursor.x;
	while (i < client->cursor.y->buffer.length && IS_CHARACTER(client->cursor.y->buffer.data[i])) i++;
	intmax_t end = i;

	buffer_set_data(&client->last_find, &client->cursor.y->buffer.data[start], end - start);
	vidd_repeat_last_find(client);
}
void vidd_find_prev_word_under_cursor(struct vidd_client* client)
{
	if (!IS_CHARACTER(client->cursor.y->buffer.data[client->cursor.x])) return;
	intmax_t i = client->cursor.x;
	while (i > 0 && IS_CHARACTER(client->cursor.y->buffer.data[i])) i--;
	intmax_t start = i + 1;

	i = client->cursor.x;
	while (i < client->cursor.y->buffer.length && IS_CHARACTER(client->cursor.y->buffer.data[i])) i++;
	intmax_t end = i;

	buffer_set_data(&client->last_find, &client->cursor.y->buffer.data[start], end - start);
	vidd_repeat_last_find_reverse(client);
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
	vidd_set_status(client);
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
	client->mode = VIDD_MODE_FIND_REVERSE;
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





intmax_t vidd_client_pool_get_non_floating_window_count(struct vidd_client_pool* pool)
{
	intmax_t count = 0;
	for (int i = 0; i < pool->length; i++)
		if (!pool->clients[i].isFloating)
			count++;
	return count;
}
void vidd_draw_vsplit_line(struct vidd_client* client)
{
	if (vidd_client_pool_get_non_floating_window_count(&client_pool) <= 1)
		return;
	cursor_move_to(client->x + client->width, client->y);
	for (int i = 0; i < client->height; i++)
	{
		printf(BRGB("255", "255", "255") FRGB("0", "0", "0") "|" NOSTYLE);
		if (i + 1 < client->height) printf(CURSOR_DOWN() CURSOR_LEFT());
	}
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
	vidd_draw_vsplit_line(&client_pool.clients[0]);
	vidd_redraw(vidd_get_active());
}
void vidd_client_next(struct vidd_client* client)
{
	vidd_draw_vsplit_line(&client_pool.clients[0]);
	vidd_client_pool_next_client(&client_pool);
	vidd_redraw(client);
	vidd_draw_vsplit_line(&client_pool.clients[0]);
	vidd_redraw(vidd_get_active());
}

void vidd_swap_active(void)
{
}





void vidd_open_empty(struct vidd_client* client)
{
	vidd_vsplit(client, "_-=[NONE]=-_");
}





void vidd_edit(struct vidd_client* client, char* args)
{
	char* file_name = client->file_name.data;
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
void vidd_floating_increase_height(struct vidd_client* client)
{
	intmax_t swidth, sheight;
	screen_get_size(&swidth, &sheight);
	if (client->isFloating)
	{
		if (client->y + client->height + 1 + 4 >= sheight)
			client->height = sheight - client->y - 2 - 4;
		client->height += 4;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_floating_decrease_height(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->height - 4 <= 5)
			client->height = 9;
		client->height -= 4;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_floating_increase_width(struct vidd_client* client)
{
	intmax_t swidth, sheight;
	screen_get_size(&swidth, &sheight);
	if (client->isFloating)
	{
		if (client->x + client->width + 1 + 4 >= swidth)
			client->width = swidth - client->x - 2 - 4;
		client->width += 4;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_floating_decrease_width(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->width - 4 <= 5)
			client->width = 9;
		client->width -= 4;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_move_floating_up(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->y - 2 <= 1)
			client->y = 2 + 2;
		client->y -= 2;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_move_floating_down(struct vidd_client* client)
{
	intmax_t swidth, sheight;
	screen_get_size(&swidth, &sheight);
	if (client->isFloating)
	{
		if (client->y + client->height + 1 + 2 >= sheight)
			client->y = sheight - client->height - 2 - 2;
		client->y += 2;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_move_floating_right(struct vidd_client* client)
{
	intmax_t swidth, sheight;
	screen_get_size(&swidth, &sheight);
	if (client->isFloating)
	{
		if (client->x + client->width + 1 + 4 >= swidth)
			client->x = swidth - client->width - 2 - 4;
		client->x += 4;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_move_floating_left(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->x - 4 <= 1)
			client->x = 2 + 4;
		client->x -= 4;
		vidd_reorganize_clients(&client_pool);
		vidd_redraw(client);
	}
}
void vidd_increase_master_size(struct vidd_client* client)
{
	if (client_pool.master_size + 0.05 < 1)
	{
		client_pool.master_size += 0.05;
		vidd_reorganize_clients(&client_pool);
	}
}
void vidd_decrease_master_size(struct vidd_client* client)
{
	if (client_pool.master_size - 0.05 > 0)
	{
		client_pool.master_size -= 0.05;
		vidd_reorganize_clients(&client_pool);
	}
}
void vidd_floating_window_draw_frame(struct vidd_client* client)
{
	printf(BRGB("0", "0", "0") FRGB("255", "255", "255"));
	cursor_move_to(client->x - 1, client->y - 1);
	printf("┌");
	for (intmax_t i = 0; i < client->width; i++)
		printf("─");

	cursor_move_to(client->x - 1, client->y + client->height);
	printf("└");
	for (intmax_t i = 0; i < client->width; i++)
		printf("─");

	cursor_move_to(client->x - 1, client->y);
	for (intmax_t i = 0; i < client->height; i++)
		printf("│" CURSOR_DOWN() CURSOR_LEFT());

	cursor_move_to(client->x + client->width, client->y - 1);
	printf("┐" CURSOR_LEFT() CURSOR_DOWN());
	for (intmax_t i = 0; i < client->height + 1; i++)
		printf("│" CURSOR_DOWN() CURSOR_LEFT());

	cursor_move_to(client->x + client->width, client->y + client->height);
	printf("┘");

}
void vidd_reorganize_clients(struct vidd_client_pool* pool)
{
	struct vidd_client* master = &pool->clients[0];

	screen_get_size(&master->width, &master->height);
	master->x = 0;
	master->y = 0;

	intmax_t screen_width = master->width;

	if (vidd_client_pool_get_non_floating_window_count(&client_pool) > 1)
		master->width *= pool->master_size;

	vidd_redraw(master);
	vidd_draw_vsplit_line(master);

	intmax_t sub_clients = vidd_client_pool_get_non_floating_window_count(&client_pool)-1;

	intmax_t sw_height = (sub_clients == 0) ? (0) : (master->height/sub_clients);
	intmax_t sw_width = screen_width - master->width - 1;
	intmax_t sw_x = master->width + 1;
	intmax_t new_y = 0;

	intmax_t extra = master->height - sw_height * sub_clients;

	for (intmax_t i = 1; i < pool->length; i++)
	{
		struct vidd_client* cur = &pool->clients[i];
		if (cur->isFloating) continue;
		cur->x = sw_x;
		cur->y = new_y;
		cur->width = sw_width;
		cur->height = sw_height;
		new_y += (extra-- > 0) ? (++cur->height) : (cur->height);
		vidd_redraw(cur);
	}
	for (intmax_t i = 1; i < pool->length; i++)
	{
		struct vidd_client* cur = &pool->clients[i];
		if (cur->isFloating)
		{
			vidd_redraw(cur);
			continue;
		}
	}
	vidd_cursor_adjust(master);
}
void vidd_swap(struct vidd_client* client)
{
	if (client->isFloating) return;
	if (client == &client_pool.clients[0])
	{
		if (client_pool.length == 1) return;
		client = &client_pool.clients[1];
	}
	struct vidd_client tmp = client_pool.clients[0];
	client_pool.clients[0] = *client;
	*client = tmp;

	client_pool.active = 0;

	vidd_reorganize_clients(&client_pool);
}
void vidd_duplicate(struct vidd_client* client)
{
	vidd_vsplit(client, "");
}
void vidd_vsplit(struct vidd_client* client, char* args)
{
	char* file_name = args;
	if (strlen(args) == 0)
	{
		file_name = client->file_name.data;
	}
	
	struct vidd_client new_client = make_vidd_client(file_name, 0, 0, 0, 0);

	struct vidd_client* new_client_ptr = vidd_client_pool_add(&client_pool, new_client);

	if (client->file_name.data == file_name)
	{
		new_client_ptr->text = client->text;
		new_client_ptr->cursor.y = client->cursor.y;
		new_client_ptr->cursor.x = client->cursor.x;
	}
	else vidd_load_file(new_client_ptr, file_name);

	vidd_reorganize_clients(&client_pool);
}
void vidd_run_command_in_vsplit(struct vidd_client* client, char* args)
{
	vidd_vsplit(client, "_-=[NONE]=-_");
	struct vidd_client* new_client = &client_pool.clients[client_pool.length-1];

	FILE* fp = popen(args, "r");
	vidd_load_from_fp(new_client, fp);
	pclose(fp);
	vidd_redraw(new_client);
}
void vidd_open_in_floating_window(struct vidd_client* client, char* args)
{
	char* file_name = args;
	if (strlen(args) == 0)
	{
		file_name = client->file_name.data;
	}

	vidd_client_pool_add(&client_pool,
				make_vidd_client(
					file_name,
					10, 10,
					client->view.width / 2,
					client->view.height / 2));

	struct vidd_client* new_client = &client_pool.clients[client_pool.length-1];

	new_client->isFloating = true;

	vidd_client_pool_set_active(&client_pool, new_client);

	vidd_load_file(new_client, file_name);
	vidd_redraw(new_client);
	vidd_enter_normal_mode(client);

}
void vidd_run_command_in_floating_window(struct vidd_client* client, char* args)
{
	intmax_t borderw = client->width / 23;
	intmax_t borderh = client->height / 10;


	vidd_client_pool_add(&client_pool,
				make_vidd_client(
					"_-=[NONE]=-_",
					(client->x + borderw), (client->y + borderh),
					client->view.width - (2 * borderw),
					client->view.height - (2 * borderh)));

	struct vidd_client* new_client = &client_pool.clients[client_pool.length-1];

	new_client->isFloating = true;

	vidd_client_pool_set_active(&client_pool, new_client);

	FILE* fp = popen(args, "r");
	vidd_load_from_fp(new_client, fp);
	pclose(fp);
	vidd_redraw(new_client);
	vidd_enter_normal_mode(client);
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
	char* file_name = client->file_name.data;
	if (strlen(args) > 0)
		file_name = args;

	if (!strcmp(file_name, "_-=[NONE]=-_")) return;

	FILE* fp = fopen(file_name, "w");
	if (fp == 0)
	{
		exit(0);
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
	free_vidd_client(client);
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

void vidd_quit_current(struct vidd_client* client)
{
	if (client_pool.length == 1) vidd_exit(client, 0);

	vidd_client_pool_remove(&client_pool, client);

	vidd_client_pool_next_client(&client_pool);
	
	vidd_reorganize_clients(&client_pool);
}
void vidd_client_quit(struct vidd_client* client, char* args)
{
	if (client_pool.length == 1) vidd_exit(client, 0);

	vidd_client_pool_remove(&client_pool, client);

	vidd_client_pool_next_client(&client_pool);
	
	vidd_reorganize_clients(&client_pool);
}
void vidd_exit(struct vidd_client* client, char* args)
{
	getch_exit();
	screen_restore();
	exit(0);
}





void vidd_void(struct vidd_client* client) {}

#endif
