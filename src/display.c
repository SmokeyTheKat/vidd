#include <vidd/display.h>

#include <vidd/commands.h>
#include <vidd/getch.h>
#include <vidd/config.h>
#include <vidd/syntax.h>
#include <vidd/config_syntax.h>

static void vidd_show_message_fmt(struct vidd_client* client, char* message, char* fmt);

void vidd_redraw(struct vidd_client* client)
{
	if (!client->displayOn) return;

	vidd_cursor_adjust(client, false);

	vidd_view_adjust_offset(client);

	struct line* line = client->text;
	intmax_t line_count = line_get_last(client->cursor.y)->number;
	intmax_t line_number_gap = number_get_length(line_count);
	intmax_t visable_line_count = client->view.height;

	struct buffer toprint = make_buffer(client->view.height * client->view.width * 4);

	if (client->view.y < 0)
	{
		buffer_printf(&toprint, CURSOR_TO("%d", "%d"), client->y, client->x + 1);
		for (intmax_t i = client->view.y; i < 0; i++)
		{
			buffer_printf(&toprint, STYLE_LINE_NUMBER_COLOR);
			buffer_push(&toprint, '*');
			buffer_push_repeat(&toprint, ' ', client->view.width + line_number_gap);
			if (i + 1 < 0)
				buffer_printf(&toprint, ((client->x != 0) ? ("\r" CURSOR_DOWN() CURSOR_RIGHT("%d")) : ("\r" CURSOR_DOWN())), client->x);
			buffer_print(&toprint, NOSTYLE);
		}
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
		if (client->numbersOn)
		{
			buffer_push_repeat(&toprint, ' ', line_number_gap - number_get_length(line->number));
	
			const intmax_t line_style_length = sizeof(STYLE_LINE_NUMBER) - 1 - 2;
	
			buffer_printf(&toprint, STYLE_LINE_NUMBER_COLOR);
			buffer_printf(&toprint, STYLE_LINE_NUMBER, line->number);
			buffer_printf(&toprint, NOSTYLE, line->number);
	
			if (client->view.xo > line_number_gap + line_style_length)
				buffer_printf(&toprint, CURSOR_RIGHT("%d"), client->x);
		}

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
		buffer_printf(&toprint, STYLE_LINE_NUMBER_COLOR);
		buffer_push(&toprint, '*');
		buffer_push_repeat(&toprint, ' ', client->view.width + line_number_gap);
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

const char* SPACES = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ";

static void vidd_show_message_fmt(struct vidd_client* client, char* message, char* fmt)
{
	cursor_save();

	printf(
		CURSOR_TO("%d", "%d") "%s%.*s",
		(int)(client->y + client->height - 1 + 1), (int)(client->x + 1), 
		fmt,
		(int)client->width, SPACES
	);

	printf(
		CURSOR_TO("%d", "%d") "%s%s" NOSTYLE, 
		(int)(client->y + client->height - 1 + 1), (int)(client->x + 1), 
		fmt,
		message
	);

	cursor_restore();

	uint32_t key = getch(false);
	vidd_interrupt(client, key);

	vidd_set_status(client);
}

void vidd_show_message(struct vidd_client* client, char* message)
{
	char* fmt = (client == vidd_get_active()) ? ACTIVE_CLIENT_COLOR : INACTIVE_CLIENT_COLOR;
	vidd_show_message_fmt(client, message, fmt);
}

void vidd_show_error(struct vidd_client* client, char* message)
{
	char* fmt = BRGB("255", "0", "0") FRGB("255", "255", "255");
	vidd_show_message_fmt(client, message, fmt);
}

void vidd_set_status(struct vidd_client* client)
{
	char* fmt = (client == vidd_get_active()) ? ACTIVE_CLIENT_COLOR : INACTIVE_CLIENT_COLOR;

	cursor_save();

	printf(
		CURSOR_TO("%d", "%d") "%s%.*s",
		(int)(client->y + client->height - 1 + 1), (int)(client->x + 1), 
		fmt,
		(int)client->width, SPACES
	);

	printf(
		CURSOR_TO("%d", "%d") "%s%s%s" NOSTYLE, 
		(int)(client->y + client->height - 1 + 1), (int)(client->x + 1), 
		fmt,
		(macro_recording == true) ? ("@") : (""),
		vidd_mode_texts[client->mode]
	);

	printf(
		CURSOR_TO("%d", "%d") "%s%s%s" NOSTYLE,
		(int)(client->y + client->height - 1 + 1), (int)(client->x + client->width - strlen(client->file_name.data) - 1 + client->unsavedChanges + 1),
		fmt,
		(client->unsavedChanges) ? "*" : "",
		client->file_name.data
	);

	cursor_restore();
}
