#include <vidd/vidd.h>

#include <vidd/config.h>
#include <vidd/themes.h>
#include <vidd/layouts.h>
#include <vidd/style.h>
#include <vidd/display.h>
#include <vidd/syntax.h>
#include <vidd/getch.h>
#include <vidd/commands.h>
#include <vidd/mode_command.h>

#include <stdlib.h>
#include <locale.h>
#include <signal.h>

struct buffer command_input;
struct buffer copy_buffer;
struct buffer run_buffer;
struct buffer macro_buffer;
bool macro_recording = false;

static char* vidd_mode_names[] = {
	[VIDD_MODE_NORMAL]="NORMAL",
	[VIDD_MODE_COMMAND]="COMMAND",
	[VIDD_MODE_WINDOW_MOVE]="WINDOW MOVE",
	[VIDD_MODE_INSERT]="INSERT",
	[VIDD_MODE_REPLACE]="REPLACE",
	[VIDD_MODE_SELECT]="SELECT",
	[VIDD_MODE_LINE_SELECT]="LINE SELECT",
};
int vidd_mode_names_length = sizeof(vidd_mode_names);

char* vidd_mode_texts[] = {
	[VIDD_MODE_NORMAL]="NORMAL",
	[VIDD_MODE_COMMAND]="COMMAND",
	[VIDD_MODE_WINDOW_MOVE]="WINDOW MOVE",
	[VIDD_MODE_INSERT]="INSERT",
	[VIDD_MODE_REPLACE]="REPLACE",
	[VIDD_MODE_SELECT]="SELECT",
	[VIDD_MODE_LINE_SELECT]="LINE SELECT",
};
int vidd_mode_texts_length = sizeof(vidd_mode_texts);

void vidd_set_mode_texts_names(void)
{
	static char normal_mode_buffer[1024] = {0};
	static char command_mode_buffer[1024] = {0};
	static char window_move_mode_buffer[1024] = {0};
	static char insert_mode_buffer[1024] = {0};
	static char replace_mode_buffer[1024] = {0};
	static char select_mode_buffer[1024] = {0};
	static char line_select_mode_buffer[1024] = {0};

	sprintf(normal_mode_buffer, active_theme->mode_format, vidd_mode_names[VIDD_MODE_NORMAL]);
	sprintf(command_mode_buffer, active_theme->mode_format, vidd_mode_names[VIDD_MODE_COMMAND]);
	sprintf(window_move_mode_buffer, active_theme->mode_format, vidd_mode_names[VIDD_MODE_WINDOW_MOVE]);
	sprintf(insert_mode_buffer, active_theme->mode_format, vidd_mode_names[VIDD_MODE_INSERT]);
	sprintf(replace_mode_buffer, active_theme->mode_format, vidd_mode_names[VIDD_MODE_REPLACE]);
	sprintf(select_mode_buffer, active_theme->mode_format, vidd_mode_names[VIDD_MODE_SELECT]);
	sprintf(line_select_mode_buffer, active_theme->mode_format, vidd_mode_names[VIDD_MODE_LINE_SELECT]);

	vidd_mode_texts[VIDD_MODE_NORMAL] = normal_mode_buffer;
	vidd_mode_texts[VIDD_MODE_COMMAND] = command_mode_buffer;
	vidd_mode_texts[VIDD_MODE_WINDOW_MOVE] = window_move_mode_buffer;
	vidd_mode_texts[VIDD_MODE_INSERT] = insert_mode_buffer;
	vidd_mode_texts[VIDD_MODE_REPLACE] = replace_mode_buffer;
	vidd_mode_texts[VIDD_MODE_SELECT] = select_mode_buffer;
	vidd_mode_texts[VIDD_MODE_LINE_SELECT] = line_select_mode_buffer;
}

char vidd_is_real_file(struct vidd_client* client)
{
	return strcmp(client->file_name.data, "_-=[NONE]=-_");
}

struct vidd_client* vidd_get_active(void)
{
	return &active_tab->clients[active_tab->active];
}

struct vidd_client make_vidd_client(char* file_name, intmax_t x, intmax_t y, intmax_t width, intmax_t height, int* open_buffers)
{
	struct vidd_client client = {0};
	client.width = width;
	client.height = height;
	client.x = x;
	client.y = y;
	if (open_buffers)
	{
		client.open_buffers = open_buffers;
		(*client.open_buffers)++;
	}
	else
	{
		client.open_buffers = malloc(sizeof(int));
		*client.open_buffers = 1;
	}
	client.syntax = syntax_c;
	client.numbersOn = 1;
	client.syntaxOn = 1;
	client.isFloating = 0;
	client.inclusiveSelection = 1;
	client.displayOn = 1;
	client.outputTabs = 1;
	client.readOnly = 0;
	client.mode = VIDD_MODE_NORMAL;
	client.file_name = make_buffer(255);
	client.make_command = make_buffer(255);
	client.last_find = make_buffer(255);
	buffer_set_data(&client.file_name, file_name, strlen(file_name));
	client.text = new_line(0);
	struct cursor cursor = { 0, 0, client.text };
	client.cursor = cursor;
	client.view.width = client.width;
	client.view.height = client.height - 1;
	client.keybinds = vidd_editor_keybinds;
	return client;
}
void free_vidd_client(struct vidd_client* client)
{
	if (*client->open_buffers == 1)
	{
		free_buffer(&client->file_name);
		vidd_text_clear(client);
		free_line(client->text);
		free(client->open_buffers);
	}
	else (*client->open_buffers)--;
	*client = (struct vidd_client){0};
}
intmax_t vidd_get_line_count(struct vidd_client* client)
{
	return line_get_last(client->cursor.y)->number;
}
intmax_t vidd_get_line_number_gap(struct vidd_client* client)
{
	return number_get_length(vidd_get_line_count(client));
}
bool vidd_is_select_mode(struct vidd_client* client)
{
	return (client->mode == VIDD_MODE_LINE_SELECT) || (client->mode == VIDD_MODE_SELECT);
}

void vidd_load_from_fp(struct line* line, FILE* fp)
{
	if (fp == 0) return;
	char buffer[4096] = {0};
	while (fgets(buffer, sizeof(buffer), fp))
	{
		for (intmax_t i = 0; buffer[i] && i < sizeof(buffer); i++)
		{
			if (buffer[i] == '\r') i++;
			if (buffer[i] == '\n')
			{
				line = line_insert_dont_adjust_line_number(line);
			}
			else if (buffer[i] == '\t') 
			{
				line_append(line, ' ');
				line_append(line, ' ');
				line_append(line, ' ');
				line_append(line, ' ');
			}
			else line_append(line, buffer[i]);
		}
	}
}

void vidd_load_stdin(struct vidd_client* client)
{
	buffer_set_data(&client->file_name, "_-=[NONE]=-_", strlen("_-=[NONE]=-_"));
	vidd_load_from_fp(client->text, stdin);
	freopen("/dev/tty", "rw", stdin);
}

void vidd_load_file(struct vidd_client* client, char* file_name)
{
	buffer_set_data(&client->file_name, file_name, strlen(file_name));
	FILE* fp = fopen(file_name, "r");
	if (fp)
	{
		vidd_load_from_fp(client->text, fp);
		vidd_syntax_ftdetect(client);
		fclose(fp);
		vidd_load_file_data(client);
	}
}


void vidd_insert_char(struct vidd_client* client)
{
	uint32_t chr = client->key;
	if (chr == '\t')
	{
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);

		vidd_redraw(client);
	}
	else
	{
		line_insert_at(client->cursor.y, chr, client->cursor.x++);
		vidd_redraw(client);
	}
}

void vidd_interrupt(struct vidd_client* client, uint32_t key)
{
	client->key = key;
	client->keybinds[client->mode](client);
	client->lkey = key;
}
void vidd_continue_input(struct vidd_client* client)
{
	uint32_t key = key = getch(client->mode == VIDD_MODE_INSERT);
	vidd_interrupt(client, key);
}

void vidd_main(void)
{
	vidd_set_mode_texts_names();
	command_input = make_buffer(150);
	copy_buffer = make_buffer(2048);
	run_buffer = make_buffer(2048);
	macro_buffer = make_buffer(2048);
	vidd_load_copy();
	vidd_arrange_clients(vidd_get_active()->tab);
	while (1) vidd_continue_input(vidd_get_active());
}


void signal_catch(int ipar)
{

}
