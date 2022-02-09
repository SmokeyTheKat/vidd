#include <vidd/display.h>

#include <vidd/commands.h>
#include <vidd/themes.h>
#include <vidd/style.h>
#include <vidd/getch.h>
#include <vidd/config.h>
#include <vidd/syntax.h>

#include <time.h>
#include <unistd.h>

static void vidd_show_message_fmt(struct vidd_client* client, char* message, char* fmt);

static const char* SPACES = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ";

void vidd_set_cursor(struct vidd_client* client)
{
	cursor_move_to(vidd_view_get_absolute_x_offset(client) + client->cursor.x - client->view.x,
				vidd_view_get_absolute_y_offset(client) + (client->cursor.y->number - 1) - client->view.y);
}

void vidd_redraw(struct vidd_client* client)
{
	if (!client->displayOn) return;

	vidd_cursor_adjust(client, false);

	vidd_view_adjust_offset(client);

	struct line* line = client->text;
	intmax_t line_number_gap = vidd_get_line_number_gap(client);
	intmax_t visable_line_count = client->view.height;

	struct buffer toprint = make_buffer(client->view.height * client->view.width * 4);
	buffer_push_default_style(&toprint);

	if (client->view.y < 0)
	{
		buffer_printf(&toprint, CURSOR_TO("%d", "%d"), client->y, client->x + 1);
		for (intmax_t i = client->view.y; i < 0; i++)
		{
			buffer_print(&toprint, active_theme->empty_line_format);
			buffer_push_repeat(&toprint, ' ', client->view.width + line_number_gap);
			if (i + 1 < 0)
				buffer_printf(&toprint, ((client->x != 0) ? ("\r" CURSOR_DOWN() CURSOR_RIGHT("%d")) : ("\r" CURSOR_DOWN())), client->x);
			buffer_push_default_style(&toprint);
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
			buffer_print(&toprint, active_theme->line_number_spacer_style);
			buffer_push_repeat(&toprint, ' ', line_number_gap - number_get_length(line->number));
	
			buffer_printf(&toprint, active_theme->line_number_format, line->number);
		}

		buffer_push_default_style(&toprint);

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
		buffer_printf(&toprint, active_theme->empty_line_format);
		buffer_push_repeat(&toprint, ' ', client->view.width + line_number_gap);
		if (i + 1 < visable_line_count)
			buffer_printf(&toprint, ((client->x != 0) ? ("\r" CURSOR_DOWN() CURSOR_RIGHT("%d")) : ("\r" CURSOR_DOWN())), client->x);
		buffer_push_default_style(&toprint);
	}

	if (client->isFloating)
		vidd_floating_window_draw_frame(client);

	printf(NOSTYLE CURSOR_HIDE "%s" CURSOR_SHOW, toprint.data);

	free_buffer(&toprint);

	if (vidd_is_select_mode(client))
		vidd_selection_draw(client);

	vidd_set_status(client);

	vidd_set_cursor(client);
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
			buffer_printf(&toprint, active_theme->highlight_style);
			if (client->view.x < line->buffer.length)
				buffer_printf(&toprint, "%.*s", (int)client->view.width, &line->buffer.data[client->view.x]);
			if (line->buffer.length == 0)
				buffer_printf(&toprint, BRGB("255", "255", "255") " ");
			buffer_printf(&toprint, NOSTYLE);
			line = line->next;
		});
	}
	else if (client->mode == VIDD_MODE_SELECT)
	{
		VIDD_SELECTION_LOOP_CRSMEJ({
			buffer_printf(&toprint, CURSOR_TO("%d", "%d"),
						vidd_view_get_absolute_y_offset(client) + i - client->view.y + 1,
						vidd_view_get_absolute_x_offset(client) + 1);
		}, {// range 
			if (x0 != 0)
				buffer_printf(&toprint, CURSOR_RIGHT("%d"), x0 - client->view.x);
			buffer_printf(&toprint, "%s%.*s" NOSTYLE, active_theme->highlight_style, (int)MIN(client->view.width, x1-x0), &line->buffer.data[x0]);
		}, {// start
			if (x0 != 0)
				buffer_printf(&toprint, CURSOR_RIGHT("%d"), x0 - client->view.x);
			buffer_printf(&toprint, "%s%.*s" NOSTYLE, active_theme->highlight_style, (int)MIN(client->view.width, line->buffer.length-x0+1), &line->buffer.data[x0]);
		}, {// middle
			buffer_printf(&toprint, "%s%.*s" NOSTYLE, active_theme->highlight_style, (int)MIN(client->view.width, line->buffer.length), line->buffer.data);
			if (line->buffer.length == 0)
				buffer_printf(&toprint, BRGB("255", "255", "255") " ");
		}, {// end
			buffer_printf(&toprint, "%s%.*s" NOSTYLE, active_theme->highlight_style, (int)MIN(client->view.width, x1), line->buffer.data);
			if (line->buffer.length == 0)
				buffer_printf(&toprint, BRGB("255", "255", "255") " ");
		}, {});
	}
	printf("%s", toprint.data);
	cursor_restore();
	free_buffer(&toprint);
}

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

//    uint32_t key = getch(false);
//    (void)key;
//    vidd_interrupt(client, key);
	usleep(300000);

	vidd_set_status(client);
}

void vidd_show_message(struct vidd_client* client, char* message)
{
	if (!client->displayOn) return;

	char* fmt = (client == vidd_get_active()) ? active_theme->status_bar_active_style : active_theme->status_bar_inactive_style;
	vidd_show_message_fmt(client, message, fmt);
}

void vidd_show_error(struct vidd_client* client, char* error)
{
	if (!client->displayOn) return;

	char* fmt = BRGB("255", "0", "0") FRGB("255", "255", "255");
	vidd_show_message_fmt(client, error, fmt);
}

void vidd_set_status(struct vidd_client* client)
{
	if (!client->displayOn) return;

	char* fmt = (client == vidd_get_active()) ? active_theme->status_bar_active_style : active_theme->status_bar_inactive_style;

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
