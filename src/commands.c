#include <vidd/commands.h>

#include <vidd/config.h>
#include <vidd/layouts.h>
#include <vidd/tab.h>
#include <vidd/display.h>
#include <vidd/getch.h>
#include <vidd/utils.h>
#include <vidd/fuzzy_find.h>
#include <vidd/syntax.h>
#include <vidd/themes.h>
#include <vidd/style.h>

#include <stdlib.h>
#include <ctype.h>

void vidd_redraw_line(struct vidd_client* client)
{
	cursor_erase_line();
	struct line* line = client->cursor.y;

	cursor_move_to(vidd_view_get_absolute_x_offset(client), client->cursor.y->number - 1);
	printf("%.*s", (int)client->view.width, line->buffer.data + client->view.x);

	vidd_set_status(client);
	vidd_cursor_adjust(client, true);
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
		vidd_arrange_clients(client->tab);
}




void vidd_toggle_case(struct vidd_client* client)
{
	char* letter = &client->cursor.y->buffer.data[client->cursor.x];
	if (isupper(*letter))
		*letter = tolower(*letter);
	else if (islower(*letter))
		*letter = toupper(*letter);

	vidd_move_right(client);
	vidd_redraw(client);
}





void vidd_switch_to_tab1(struct vidd_client* client)
{
	vidd_tab_set(1);
}
void vidd_switch_to_tab2(struct vidd_client* client)
{
	vidd_tab_set(2);
}
void vidd_switch_to_tab3(struct vidd_client* client)
{
	vidd_tab_set(3);
}
void vidd_switch_to_tab4(struct vidd_client* client)
{
	vidd_tab_set(4);
}
void vidd_switch_to_tab5(struct vidd_client* client)
{
	vidd_tab_set(5);
}
void vidd_switch_to_tab6(struct vidd_client* client)
{
	vidd_tab_set(6);
}
void vidd_switch_to_tab7(struct vidd_client* client)
{
	vidd_tab_set(7);
}
void vidd_switch_to_tab8(struct vidd_client* client)
{
	vidd_tab_set(8);
}
void vidd_switch_to_tab9(struct vidd_client* client)
{
	vidd_tab_set(9);
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

	{
		intmax_t last_movable = vidd_last_movable(client);
		if (client->cursor.x >= last_movable)
			client->cursor.x = last_movable;
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
void vidd_cursor_adjust(struct vidd_client* client, bool can_redraw)
{
	bool redraw = false;

	vidd_view_adjust_offset(client);
	if (vidd_cursor_fix_overflow(client) && !redraw) redraw = true;
	if (vidd_view_fix_overflow(client) && !redraw) redraw = true;
	if (vidd_selection_redraw_needed(client) && !redraw) redraw = true;

	if (redraw && can_redraw) vidd_redraw(client);
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
	vidd_cursor_adjust(client, true);
}
void vidd_move_down(struct vidd_client* client)
{
	if (client->cursor.y->next)
		client->cursor.y = client->cursor.y->next;
	client->cursor.x = client->cursor.lx;
	vidd_cursor_adjust(client, true);
}
void vidd_move_right(struct vidd_client* client)
{
	if (client->cursor.x == vidd_last_movable(client))
	{
		//printf("\a");
		return;
	}
	client->cursor.x++;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client, true);
}
void vidd_move_left(struct vidd_client* client)
{
	if (client->cursor.x == 0)
	{
		//printf("\a");
		return;
	}
	client->cursor.x--;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client, true);
}
void vidd_move_to_line_start(struct vidd_client* client)
{
	client->cursor.x = 0;
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client, true);
}
void vidd_move_to_line_end(struct vidd_client* client)
{
	client->cursor.x = vidd_last_movable(client);
	client->cursor.lx = 696969696969696969;
	vidd_cursor_adjust(client, true);
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
	vidd_cursor_adjust(client, true);
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
	vidd_cursor_adjust(client, true);
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
	vidd_cursor_adjust(client, true);
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
	if (client->numbersOn)
	{
		const intmax_t line_style_length = visable_strlen(active_theme->line_number_format) - 2;
		client->view.height = client->height - 1;
		intmax_t number_length = number_get_length(line_get_last(client->text)->number);
		if (client->view.xo != number_length + line_style_length ||
			client->view.width != client->width - number_length + line_style_length)
		{
			client->view.xo = number_length + line_style_length;
			client->view.width = client->width - client->view.xo;
		}
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
	uint32_t c = getch(true);
	client->marker.positions[c].x = client->cursor.x;
	client->marker.positions[c].y = client->cursor.y->number;
}
void vidd_goto_marker(struct vidd_client* client)
{
	uint32_t c = getch(true);
	vidd_move_to(client, client->marker.positions[c].x, client->marker.positions[c].y);
}
void vidd_save_spot(struct vidd_client* client)
{
	client->marker.positions[0].x = client->cursor.x;
	client->marker.positions[0].y = client->cursor.y->number;
}
void vidd_load_spot(struct vidd_client* client)
{
	vidd_move_to(client, client->marker.positions[0].x, client->marker.positions[0].y);
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
	vidd_cursor_adjust(client, true);
}
void vidd_move_to(struct vidd_client* client, intmax_t x, intmax_t y)
{
	client->cursor.x = x;
	client->cursor.y = line_get_line(client->cursor.y, y);
	client->cursor.lx = client->cursor.x;
	vidd_cursor_adjust(client, true);
}




void vidd_macro_toggle(struct vidd_client* client)
{
	if (macro_recording == false)
		buffer_clear(&macro_buffer);
	macro_recording = !macro_recording;
	vidd_set_status(client);
}
void vidd_macro_run(struct vidd_client* client)
{
	buffer_copy_inverse(&run_buffer, &macro_buffer);
	buffer_pop_front(&run_buffer);
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





void vidd_toggle_comment(struct vidd_client* client)
{
	if (*(uint16_t*)client->cursor.y->buffer.data == *(uint16_t*)"//")
	{
		line_delete_at(client->cursor.y, 0);
		line_delete_at(client->cursor.y, 0);
	}
	else
	{
		line_insert_at(client->cursor.y, '/', 0);
		line_insert_at(client->cursor.y, '/', 0);
	}
	vidd_redraw(client);
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





void vidd_man_word(struct vidd_client* client)
{
	if (!IS_CHARACTER(client->cursor.y->buffer.data[client->cursor.x])) return;
	intmax_t i = client->cursor.x;
	while (i > 0 && IS_CHARACTER(client->cursor.y->buffer.data[i])) i--;
	intmax_t start = i + 1;

	i = client->cursor.x;
	while (i < client->cursor.y->buffer.length && IS_CHARACTER(client->cursor.y->buffer.data[i])) i++;
	intmax_t end = i;

	struct buffer word = make_buffer(64);
	buffer_push_cstring(&word, &client->cursor.y->buffer.data[start], end - start);

	vidd_man(client, word.data);
	free_buffer(&word);
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
		bool fist_line_deleted = false;
		VIDD_LINE_SELECTION_FULL_LOOP({
			if (line == client->text) fist_line_deleted = true;
			line = line_remove(line);
		});
		client->cursor.y = line;
		if (fist_line_deleted)
			client->text = line;
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
	char key = getch(true);
	client->cursor.y->buffer.data[client->cursor.x] = key;
	vidd_redraw(client);
}




void vidd_selection_toggle_comment(struct vidd_client* client)
{
	vidd_toggle_drawing(client);
	struct cursor old_cursor = client->cursor;
	struct line* line;
	VIDD_LINE_SELECTION_FULL_LOOP({
		vidd_move_to(client, 0, i+1);
		vidd_toggle_comment(client);
	});
	client->cursor = old_cursor;
	vidd_toggle_drawing(client);
}
void vidd_selection_indent(struct vidd_client* client)
{
	vidd_toggle_drawing(client);
	struct cursor old_cursor = client->cursor;
	struct line* line;
	VIDD_LINE_SELECTION_FULL_LOOP({
		vidd_move_to(client, 0, i+1);
		vidd_indent(client);
	});
	client->cursor = old_cursor;
	vidd_toggle_drawing(client);
}
void vidd_selection_deindent(struct vidd_client* client)
{
	vidd_toggle_drawing(client);
	struct cursor old_cursor = client->cursor;
	struct line* line;
	VIDD_LINE_SELECTION_FULL_LOOP({
		vidd_move_to(client, 0, i+1);
		vidd_deindent(client);
	});
	client->cursor = old_cursor;
	vidd_toggle_drawing(client);

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

	vidd_cursor_adjust(client, true);
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
		printf(active_theme->highlight_style);
		if (client->view.x < line->buffer.length)
			printf("%.*s", (int)client->view.width, &line->buffer.data[client->view.x]);
		printf(NOSTYLE);
		line = line_get_line_in_direction(line, dy);
		p += dy;
	}
	cursor_restore();
	
}




void vidd_save_copy(void)
{
	FILE* fp = fopen(PREFIX "/share/vidd/cpybuf.data", "w");
	fwrite(copy_buffer.data, 1, copy_buffer.length, fp);
	fclose(fp);
}
void vidd_load_copy(void)
{
	FILE* fp = fopen(PREFIX "/share/vidd/cpybuf.data", "r");
	char buffer[150];
	while (fread(buffer, 1, sizeof(buffer), fp) != 0)
		buffer_print(&copy_buffer, buffer);
	fclose(fp);
}
void vidd_copy_to_clipboard(struct vidd_client* client)
{
	struct buffer transfer = make_buffer(copy_buffer.length);
	for (int i = 0; i < copy_buffer.length; i++)
	{
/*
		if (copy_buffer.data[i]))
			buffer_push(&transfer, copy_buffer.data[i]);
*/
		char c = copy_buffer.data[i];
		if (c == 'i' || c == 'o')
		{
			i++;
			for (; i < copy_buffer.length && copy_buffer.data[i] != 27; i++)
				buffer_push(&transfer, copy_buffer.data[i]);
		}
	}

	char* com_front = "echo '";
	char* com_end = "' | xclip -selection c";
	buffer_push_cstring_front(&transfer, com_front, strlen(com_front));
	buffer_push_cstring(&transfer, com_end, strlen(com_end));
	FILE* fp = popen(transfer.data, "r");
	pclose(fp);
	free_buffer(&transfer);
}
void vidd_copy_line(struct vidd_client* client)
{
	buffer_clear(&copy_buffer);
	buffer_push(&copy_buffer, 27);
	buffer_push(&copy_buffer, 'o');
	buffer_push_cstring(&copy_buffer, client->cursor.y->buffer.data, client->cursor.y->buffer.length);
	buffer_push(&copy_buffer, 27);
	buffer_push(&copy_buffer, '^');
	vidd_save_copy();
}
void vidd_selection_copy(struct vidd_client* client)
{
	struct line* line;

	buffer_clear(&copy_buffer);
	buffer_push(&copy_buffer, 27);
	if (client->mode == VIDD_MODE_LINE_SELECT)
	{
		buffer_push(&copy_buffer, 253);
		buffer_push(&copy_buffer, 'o');

		VIDD_LINE_SELECTION_FULL_LOOP({
			buffer_push_cstring(&copy_buffer, line->buffer.data, line->buffer.length);
			if (i + 1 <= lim) buffer_push(&copy_buffer, 13);
			line = line->next;
		});
		buffer_push(&copy_buffer, 27);
		buffer_push(&copy_buffer, 254);
		buffer_push(&copy_buffer, 'j');
		buffer_push(&copy_buffer, '^');
	}
	else
	{
		buffer_push(&copy_buffer, 'a');

		VIDD_SELECTION_FULL_LOOP_CRSMEJ(
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
	vidd_save_copy();
}





void vidd_paste(struct vidd_client* client)
{
	vidd_toggle_drawing(client);
	for (intmax_t i = 0; i < copy_buffer.length; i++)
	{
		vidd_interrupt(client, copy_buffer.data[i]);
	}
	vidd_toggle_drawing(client);
}





void vidd_find_next_char(struct vidd_client* client)
{
	unsigned char to_find = getch(true);
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
	unsigned char to_find = getch(true);
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
	vidd_show_error(client, "no results");
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
	vidd_show_error(client, "no results");
}
void vidd_find_next_word_under_cursor(struct vidd_client* client)
{
	if (!IS_CHARACTER(client->cursor.y->buffer.data[client->cursor.x])) return;
	intmax_t i = client->cursor.x;
	while (i > 0 && IS_CHARACTER(client->cursor.y->buffer.data[i])) i--;
	intmax_t start = i + 1;

	i = client->cursor.x;
	while (i < client->cursor.y->buffer.length && IS_CHARACTER(client->cursor.y->buffer.data[i])) i++;
	if (i < client->cursor.y->buffer.length && client->cursor.y->buffer.data[i] == '(') i++;
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
	if (i < client->cursor.y->buffer.length && client->cursor.y->buffer.data[i] == '(') i++;
	intmax_t end = i;

	buffer_set_data(&client->last_find, &client->cursor.y->buffer.data[start], end - start);
	vidd_repeat_last_find_reverse(client);
}
void vidd_goto_next_paragraph(struct vidd_client* client)
{
	struct line* line = client->cursor.y;
	if (!line_is_empty(line))
	{
		while (line->next && !line_is_empty(line))
			line = line->next;
	}
	else
	{
		line = line->next;
		if (!line) return;
		while (line->next && line_is_empty(line))
			line = line->next;
		while (line->next && (!line_is_empty(line) == !line_is_empty(line->next)))
			line = line->next;
		if (!line_is_empty(line))
			while (line->next && !line_is_empty(line))
				line = line->next;
	}
	client->cursor.y = line;
	vidd_cursor_adjust(client, true);
}
void vidd_goto_prev_paragraph(struct vidd_client* client)
{
	struct line* line = client->cursor.y;
	if (!line_is_empty(line))
	{
		while (line->prev && !line_is_empty(line))
			line = line->prev;
	}
	else
	{
		line = line->prev;
		if (!line) return;
		while (line->prev && line_is_empty(line))
			line = line->prev;
		while (line->prev && (!line_is_empty(line) == !line_is_empty(line->prev)))
			line = line->prev;
		if (!line_is_empty(line))
			while (line->prev && !line_is_empty(line))
				line = line->prev;
	}
	client->cursor.y = line;
	vidd_cursor_adjust(client, true);
}





void vidd_mode_swap(struct vidd_client* client, int com)
{
	static int mode = 0;
	if (com == 1)
		client->mode = mode;
	else if (com == 0)
		mode = client->mode;
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
void vidd_exit_insert_mode(struct vidd_client* client)
{
	client->mode = VIDD_MODE_NORMAL;
	vidd_move_left(client);
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
void vidd_enter_window_move_mode(struct vidd_client* client)
{
	client->mode = VIDD_MODE_WINDOW_MOVE;
	vidd_set_status(client);
}
void vidd_enter_command_mode(struct vidd_client* client)
{
	cursor_save();
	cursor_move_to(client->x + visable_strlen(vidd_mode_texts[client->mode]), client->y + client->height - 1);
	printf(active_theme->status_bar_active_style);
	printf(":");
	client->mode = VIDD_MODE_COMMAND;
}
void vidd_enter_find_next_mode(struct vidd_client* client)
{
	cursor_save();
	cursor_move_to(client->x + visable_strlen(vidd_mode_texts[client->mode]), client->y + client->height);
	printf(active_theme->status_bar_active_style);
	printf("/");
	vidd_mode_swap(client, 0);
	client->mode = VIDD_MODE_FIND;
}
void vidd_enter_find_prev_mode(struct vidd_client* client)
{
	cursor_save();
	cursor_move_to(client->x + visable_strlen(vidd_mode_texts[client->mode]), client->y + client->height);
	printf(active_theme->status_bar_active_style);
	printf("?");
	vidd_mode_swap(client, 0);
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





void vidd_draw_vsplit_line(struct vidd_client* client)
{
	if (client->tab->length <= client->tab->master_count || client->tab->master_count <= 0)
		return;

	if (vidd_tab_get_non_floating_count(client->tab) <= 1)
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
	vidd_tab_prev_client(client->tab);
	vidd_redraw(client);
	vidd_draw_vsplit_line(&client->tab->clients[0]);
	vidd_redraw(vidd_get_active());
}
void vidd_client_next(struct vidd_client* client)
{
	vidd_draw_vsplit_line(&client->tab->clients[0]);
	vidd_tab_next_client(client->tab);
	vidd_redraw(client);
	vidd_draw_vsplit_line(&client->tab->clients[0]);
	vidd_redraw(vidd_get_active());
}

void vidd_swap_active(void)
{
}





void vidd_open_empty(struct vidd_client* client)
{
	vidd_open(client, "_-=[NONE]=-_");
}





void vidd_edit(struct vidd_client* client, char* args)
{
	char* file_name = client->file_name.data;
	if (strlen(args) > 0)
		file_name = args;

	struct vidd_client tmp = *client;
	free_vidd_client(client);
	*client = make_vidd_client(file_name, tmp.x, tmp.y, tmp.width, tmp.height, 0);
	vidd_load_file(client, file_name);
	client->view.xo = tmp.view.xo;
	client->view.yo = tmp.view.yo;
	client->view.height = tmp.view.height;
	client->view.width = tmp.view.width;
	client->width = tmp.width;
	client->height = tmp.height;
	client->x = tmp.x;
	client->y = tmp.y;
	client->tab = tmp.tab;
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
		vidd_arrange_clients(client->tab);
	}
}
void vidd_floating_decrease_height(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->height - 4 <= 5)
			client->height = 9;
		client->height -= 4;
		vidd_arrange_clients(client->tab);
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
		vidd_arrange_clients(client->tab);
	}
}
void vidd_floating_decrease_width(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->width - 4 <= 5)
			client->width = 9;
		client->width -= 4;
		vidd_arrange_clients(client->tab);
	}
}
void vidd_move_floating_up(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->y - 2 <= 1)
			client->y = 2 + 2;
		client->y -= 2;
		vidd_arrange_clients(client->tab);
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
		vidd_arrange_clients(client->tab);
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
		vidd_arrange_clients(client->tab);
	}
}
void vidd_move_floating_left(struct vidd_client* client)
{
	if (client->isFloating)
	{
		if (client->x - 4 <= 1)
			client->x = 2 + 4;
		client->x -= 4;
		vidd_arrange_clients(client->tab);
	}
}
void vidd_floating_center(struct vidd_client* client)
{
	intmax_t width, height;
	screen_get_size(&width, &height);
	client->width = width - 2 * (width / 10);
	client->height = height - 2 * (height / 10);
	client->x = width / 10;
	client->y = height / 10;
}
void vidd_floating_toggle(struct vidd_client* client)
{
	client->isFloating = !client->isFloating;
	if (client->isFloating)
		vidd_floating_center(client);
	vidd_arrange_clients(client->tab);
}
void vidd_increase_master_size(struct vidd_client* client)
{
	if (client->tab->master_size + 0.05 < 1)
	{
		client->tab->master_size += 0.05;
		vidd_arrange_clients(client->tab);
	}
}
void vidd_decrease_master_size(struct vidd_client* client)
{
	if (client->tab->master_size - 0.05 > 0)
	{
		client->tab->master_size -= 0.05;
		vidd_arrange_clients(client->tab);
	}
}
void vidd_next_layout(struct vidd_client* client)
{
	vidd_tab_next_layout(client->tab);
}





void vidd_run_line(struct vidd_client* client)
{
	struct line* output = new_line(0);
	FILE* fp = popen(client->cursor.y->buffer.data, "r");
	vidd_load_from_fp(output, fp);
	pclose(fp);
	line_join_line_strings(client->cursor.y, output);
	vidd_redraw(client);
}





void vidd_floating_window_draw_frame(struct vidd_client* client)
{
	printf("%s%s", active_theme->bg_style, active_theme->fg_style);
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
struct vidd_client* vidd_get_master_client(struct vidd_client* client)
{
	struct vidd_tab* tab = client->tab;
	for (int i = 0; i < tab->length; i++)
	{
		if (tab->clients[i].isFloating == false)
			return &tab->clients[i];
	}
	return &tab->clients[0];
}
void vidd_increase_master_count(struct vidd_client* client)
{
	client->tab->master_count++;
	vidd_arrange_clients(client->tab);
}
void vidd_decrease_master_count(struct vidd_client* client)
{
	client->tab->master_count--;
	vidd_arrange_clients(client->tab);
}
void vidd_swap(struct vidd_client* client)
{
	if (client->isFloating) return;
	if (client == &client->tab->clients[0])
	{
		if (client->tab->length == 1) return;
		client = &client->tab->clients[1];
	}
	struct vidd_client tmp = client->tab->clients[0];
	client->tab->clients[0] = *client;
	*client = tmp;

	client->tab->active = 0;

	vidd_arrange_clients(client->tab);
}
void vidd_duplicate(struct vidd_client* client)
{
	vidd_open(client, "");
}
void vidd_open(struct vidd_client* client, char* args)
{
	char* file_name = args;
	if (strlen(args) == 0)
	{
		file_name = client->file_name.data;
	}

	struct vidd_client new_client;
	
	if ((void*)client->file_name.data == (void*)file_name)
		new_client = make_vidd_client(file_name, 0, 0, 0, 0, client->open_buffers);
	else new_client = make_vidd_client(file_name, 0, 0, 0, 0, 0);

	struct vidd_client* new_client_ptr = vidd_tab_add_client(client->tab, new_client);

	if ((void*)client->file_name.data == (void*)file_name)
	{
		new_client_ptr->text = client->text;
		new_client_ptr->cursor.y = client->cursor.y;
		new_client_ptr->cursor.x = client->cursor.x;
		new_client_ptr->settings = client->settings;
		new_client_ptr->isFloating = false;
		new_client_ptr->syntax = client->syntax;
	}
	else vidd_load_file(new_client_ptr, file_name);

	vidd_swap(new_client_ptr);
}
void vidd_run_command_in_open(struct vidd_client* client, char* args)
{
	vidd_open(client, "_-=[NONE]=-_");
	struct vidd_client* new_client = &client->tab->clients[client->tab->length-1];

	FILE* fp = popen(args, "r");
	vidd_load_from_fp(new_client->text, fp);
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

	if ((void*)client->file_name.data == (void*)file_name)
		vidd_tab_add_client(client->tab,
					make_vidd_client(
						file_name,
						10, 10,
						client->view.width / 2,
						client->view.height / 2,
						client->open_buffers));
	else
		vidd_tab_add_client(client->tab,
					make_vidd_client(
						file_name,
						10, 10,
						client->view.width / 2,
						client->view.height / 2,
						0));

	struct vidd_client* new_client = &client->tab->clients[client->tab->length-1];

	new_client->isFloating = true;

	vidd_tab_set_active(client->tab, new_client);

	vidd_load_file(new_client, file_name);
	vidd_redraw(new_client);
	vidd_enter_normal_mode(client);

}
void vidd_run_command_in_floating_window(struct vidd_client* client, char* args)
{
	intmax_t borderw = client->width / 23;
	intmax_t borderh = client->height / 10;

	vidd_tab_add_client(
		client->tab,
		make_vidd_client(
			args,
			(client->x + borderw), (client->y + borderh),
			client->view.width - (2 * borderw),
			client->view.height - (2 * borderh),
			0
		)
	);

	struct vidd_client* new_client = &client->tab->clients[client->tab->length-1];

	new_client->isFloating = true;

	vidd_tab_set_active(client->tab, new_client);

	intmax_t comlen = strlen(args) + 6;
	char* com = malloc(comlen);
	memcpy(com, args, comlen-6);
	memcpy(com + comlen-6, " 2>&1", 5);
	com[comlen] = 0;
	FILE* fp = popen(com, "r");
	vidd_load_from_fp(new_client->text, fp);
	fclose(fp);
	free(com);

	vidd_redraw(new_client);
	vidd_enter_normal_mode(client);
}
void vidd_run_command(struct vidd_client* client, char* args)
{
	intmax_t comlen = strlen(args) + 20;
	char* com = malloc(comlen);
	memcpy(com, args, comlen-19);
	memcpy(com + comlen-19, " > /dev/null 2>&1 &", 19);
	com[comlen] = 0;
	FILE* fp = popen(com, "r");
	fclose(fp);
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

	intmax_t line_count = 0;
	intmax_t byte_count = 0;

	struct line* line = client->text;
	while (line)
	{
		bool nontabs_found = false;
		for (intmax_t i = 0; i < line->buffer.length; i++)
		{
			if (client->outputTabs && !nontabs_found && *(uint32_t*)&line->buffer.data[i] == *(uint32_t*)"    ")
			{
				fputc('\t', fp);
				i += 3;
			}
			else
			{
				fputc(line->buffer.data[i], fp);
				nontabs_found = true;
			}
			byte_count++;
		}
		byte_count++;
		if (line->next) fwrite("\n", 1, 1, fp);
		else line_count = line->number;
		line = line->next;
	}
	byte_count--;
	fclose(fp);
	client->unsavedChanges = false;

	char message[1024] = {0};
	sprintf(message, "%ldL %ldB written to \"%s\"", line_count, byte_count, file_name);
	vidd_show_message(client, message);
}
void vidd_write_all(struct vidd_client* client, char* args)
{
	for (intmax_t i = 0; i < client->tab->length; i++)
		vidd_write(&client->tab->clients[i], "");
}
void vidd_write_quit_all(struct vidd_client* client, char* args)
{
	if (client->tab->length == 0) return;
	vidd_write(&client->tab->clients[0], "");
	vidd_client_quit(&client->tab->clients[0], "");
	vidd_write_quit_all(client, args);
}
void vidd_write_quit(struct vidd_client* client, char* args)
{
	vidd_write(client, args);
	vidd_client_quit(client, 0);
}

struct buffer vidd_get_file_data_path(struct vidd_client* client)
{
	char real_path_buf[PATH_MAX+1];
	char* real_path = realpath(client->file_name.data, real_path_buf);

	if (real_path == 0) return (struct buffer){0};

	for (int i = 0; real_path[i]; i++)
		if (real_path[i] == '/')
			real_path[i] = '+';
	return make_buffer_format(PREFIX "/share/vidd/filedata/%s", real_path);
}

void vidd_save_file_data(struct vidd_client* client)
{
	if (!vidd_is_real_file(client)) return;
	struct buffer file_path = vidd_get_file_data_path(client);
	if (file_path.data == 0) return;

	FILE* fp = fopen(file_path.data, "w");
	fwrite(&client->cursor.x, sizeof(client->cursor.x), 1, fp);
	fwrite(&client->cursor.y->number, sizeof(client->cursor.x), 1, fp);
	fwrite(client->make_command.data, 1, 255, fp);
	fclose(fp);
	free_buffer(&file_path);
}
void vidd_load_file_data(struct vidd_client* client)
{
	if (!vidd_is_real_file(client)) return;
	struct buffer file_path = vidd_get_file_data_path(client);

	intmax_t y = 0;
	char make_command[255];
	FILE* fp = fopen(file_path.data, "r");
	if (fp)
	{
		fread(&client->cursor.x, sizeof(client->cursor.x), 1, fp);
		fread(&y, sizeof(client->cursor.x), 1, fp);
		fread(make_command, 1, 255, fp);

		buffer_set_data(&client->make_command, make_command, strlen(make_command));
		client->cursor.x = 0;
		client->cursor.y = line_get_line(client->cursor.y, y);
		vidd_redraw(client);

		fclose(fp);
	}
	free_buffer(&file_path);
}





void vidd_run_make(struct vidd_client* client)
{
	vidd_run_command_in_floating_window(client, client->make_command.data);
}





void vidd_client_quit(struct vidd_client* client, char* args)
{
	if (client->unsavedChanges && !(client->key == KEY_CTRL('c') && client->lkey == KEY_CTRL('c')))
	{
		vidd_show_error(client, "unsaved changes");
		return;
	}

	vidd_save_file_data(client);

	if (client->tab->length == 1) vidd_force_exit_all(client, 0);

	struct vidd_tab* tab = client->tab;

	vidd_tab_remove_client(client->tab, client);

	tab->active = 0;

	vidd_arrange_clients(tab);
}
void vidd_client_force_quit(struct vidd_client* client, char* args)
{
	if (client->tab->length == 1) vidd_force_exit_all(client, 0);

	vidd_tab_remove_client(client->tab, client);

	vidd_tab_next_client(client->tab);

	vidd_arrange_clients(client->tab);
}
void vidd_exit_all(struct vidd_client* client, char* args)
{
	for (intmax_t i = 0; i < client->tab->length;)
	{
		intmax_t olen = client->tab->length;
		vidd_client_quit(&client->tab->clients[i], "");
		intmax_t nlen = client->tab->length;
		if (olen == nlen) i++;
	}
}
void vidd_force_exit_all(struct vidd_client* client, char* args)
{
	printf("\x1b[0m");
	getch_exit();
	screen_restore();
	exit(0);
}
void vidd_load_syntax(struct vidd_client* client, char* args)
{
	vidd_syntax_ftdetect_cstring(client, args);
	vidd_redraw(client);
}
void vidd_man(struct vidd_client* client, char* args)
{
	struct buffer bcom = make_buffer_from_cstring(args);
	buffer_push_cstring_front(&bcom, "man ", 4);
	vidd_run_command_in_floating_window(client, bcom.data);
	free_buffer(&bcom);
}
void vidd_set(struct vidd_client* client, char* args)
{
	char* var = strtok(args, " \n\0");
	if (var == 0) return;
	if (!strcmp(var, "numbers"))
	{
		char* value = strtok(0, " \n\0");
		client->numbersOn = (value == 0 || !strcmp(value, "yes"));
		if (client->numbersOn == false)
		{
			client->view.x = client->x;
			client->view.y = client->y;
			client->view.xo = 0;
			client->view.yo = 0;
			client->view.width = client->width;
			client->view.height = client->height;
		}
		vidd_redraw(client);
	}
	else if (!strcmp(var, "display"))
	{
		char* value = strtok(0, " \n\0");
		client->displayOn = (value == 0 || !strcmp(value, "yes"));
	}
	else if (!strcmp(var, "inc-sel"))
	{
		char* value = strtok(0, " \n\0");
		client->inclusiveSelection = (value == 0 || !strcmp(value, "yes"));
	}
	else if (!strcmp(var, "output-tabs"))
	{
		char* value = strtok(0, " \n\0");
		client->outputTabs = (value == 0 || !strcmp(value, "yes"));
	}
	else if (!strcmp(var, "readonly"))
	{
		char* value = strtok(0, " \n\0");
		client->readOnly = (value == 0 || !strcmp(value, "yes"));
	}
	else if (!strcmp(var, "make"))
	{
		char* value = strtok(0, "\n\0");
		buffer_set_data(&client->make_command, value, strlen(value));
	}
	else if (!strcmp(var, "theme"))
	{
		char* value = strtok(0, "\n\0");
		for (int i = 0; i < themes_length; i++)
		{
			if (strcmp(themes[i]->name, value) == 0)
			{
				active_theme = themes[i];
				vidd_arrange_clients(client->tab);
				vidd_set_mode_texts_names();
				return;
			}
		}
	}
	else if (!strcmp(var, "title"))
	{
		char* value = strtok(0, "\n\0");
		set_terminal_title(value);
	}
}




void vidd_void(struct vidd_client* client) {}
