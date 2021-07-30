#include <signal.h>

#include "./vidd.h"

#include "./config_syntax.h"

uint32_t getch(void)
{
	if (run_buffer.length > 0)
	{
		return run_buffer.data[--run_buffer.length];
	}
	uint64_t output = 0;
	read(STDIN_FILENO, &output, 8);
	return ((output > 255) * 255) + (output % 255);
}

#include "./commands.h"

struct vidd_client* vidd_get_active(void)
{
	return &client_pool.clients[client_pool.active];
}

intmax_t vidd_client_pool_get_client_index(struct vidd_client_pool* pool, struct vidd_client* client)
{
	for (intmax_t i = 0; i < pool->length; i++)
		if (client == &pool->clients[i]) return i;
	return -1;
}

void vidd_client_pool_set_active(struct vidd_client_pool* pool, struct vidd_client* client)
{
	pool->active = vidd_client_pool_get_client_index(pool, client);
}

void vidd_client_pool_prev_client(struct vidd_client_pool* pool)
{
	pool->active--;
	if (pool->active < 0) pool->active = pool->length-1;
}

void vidd_client_pool_next_client(struct vidd_client_pool* pool)
{
	pool->active++;
	if (pool->active >= pool->length) pool->active = 0;
}

struct vidd_client_pool make_vidd_client_pool(intmax_t start_size)
{
	struct vidd_client_pool pool = { 0 };
	pool.size = start_size;
	pool.clients = malloc(sizeof(struct vidd_client) * pool.size);
	pool.master_size = 0.5;
	pool.active = 0;
	return pool;
}

struct vidd_client* vidd_client_pool_add(struct vidd_client_pool* pool, struct vidd_client client)
{
	if (pool->length + 1 > pool->size)
		pool->clients = realloc(pool->clients, ++pool->size);

	pool->clients[pool->length] = client;
	return &pool->clients[pool->length++];
}

struct vidd_client make_vidd_client(char* file_name, intmax_t x, intmax_t y, intmax_t width, intmax_t height)
{
	struct vidd_client client = {0};
	client.width = width;
	client.height = height;
	client.x = x;
	client.y = y;
	client.syntax = syntax_c;
	client.numbersOn = 1;
	client.syntaxOn = 1;
	client.isFloating = 0;
	client.inclusiveSelection = 1;
	client.displayOn = 1;
	client.mode = VIDD_MODE_NORMAL;
	client.file_name = make_buffer(150);
	client.last_find = make_buffer(150);
	buffer_set_data(&client.file_name, file_name, strlen(file_name));
	client.text = new_line(0);
	struct cursor cursor = { 0, 0, client.text };
	client.cursor = cursor;
	client.view.width = client.width;
	client.view.height = client.height - 1;
	return client;
}
void free_vidd_client(struct vidd_client* client)
{
	free_buffer(&client->file_name);
	vidd_text_clear(client);
	free_line(client->text);
	*client = (struct vidd_client){0};
}

void vidd_load_from_fp(struct vidd_client* client, FILE* fp)
{
	if (fp == 0) return;
	char buffer[1024] = {0};
	struct line* line = client->text;
	while (fgets(buffer, sizeof(line), fp))
	{
		intmax_t length = strlen(buffer);
		for (intmax_t i = 0; i < length; i++)
		{
			if (buffer[i] == '\n')
			{
				line = line_insert(line);
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
	vidd_load_from_fp(client, stdin);
	freopen("/dev/tty", "rw", stdin);
}
void vidd_load_file(struct vidd_client* client, char* file_name)
{
	buffer_set_data(&client->file_name, file_name, strlen(file_name));
	FILE* fp = fopen(file_name, "r");
	if (fp)
	{
		vidd_load_from_fp(client, fp);
		vidd_syntax_ftdetect(client);
		fclose(fp);
	}
}


void vidd_insert_char(struct vidd_client* client)
{
	uint32_t chr = client->key;
	if (chr == 27)
	{
		client->mode = VIDD_MODE_NORMAL;
		vidd_move_left(client);
		vidd_set_status(client);
	}
	else if (chr == '\t')
	{
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);
		line_insert_at(client->cursor.y, ' ', client->cursor.x++);

		vidd_redraw(client);
	}
	else if (chr == KEY_RETURN)
	{
		vidd_split_at_cursor(client);
	}
	else if (chr == 8)
	{
		struct buffer tmpbuf = { "\x1b" "bdexi", 6, 6 };
		buffer_copy_inverse(&run_buffer, &tmpbuf);
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
	switch (client->mode)
	{
		case VIDD_MODE_NORMAL:
		{
			vidd_normal_mode_keybinds[key](client);
		} break;
		case VIDD_MODE_LINE_SELECT:
		{
			if (vidd_select_mode_keybinds[key]) vidd_select_mode_keybinds[key](client);
			else vidd_normal_mode_keybinds[key](client);
		} break;
		case VIDD_MODE_SELECT:
		{
			if (vidd_select_mode_keybinds[key]) vidd_select_mode_keybinds[key](client);
			else vidd_normal_mode_keybinds[key](client);
		} break;
		case VIDD_MODE_INSERT:
		{
			vidd_insert_mode_keybinds[key](client);
		} break;
		case VIDD_MODE_REPLACE:
		{
			if (key == 27)
			{
				client->mode = VIDD_MODE_NORMAL;
				vidd_move_left(client);
				vidd_set_status(client);
			}
			else
			{
				client->cursor.y->buffer.data[client->cursor.x++] = key;
				vidd_redraw(client);
			}
		} break;
		case VIDD_MODE_COMMAND:
		{
			if (key == 27)
			{
				buffer_clear(&command_input);
				cursor_restore();
				client->mode = VIDD_MODE_NORMAL;
				vidd_set_status(client);
			}
			else if (key == 127)
			{
				if (command_input.length == 0) break;
				cursor_move(-1, 0);
				printf(" ");
				cursor_move(-1, 0);
				buffer_pop(&command_input);
			}
			else if (key == KEY_RETURN)
			{
				//dbs_log(0, command_input.data);
				char* command = strtok(command_input.data, " \n\t\0 ");
				bool command_found = false;
				for (intmax_t i = 0; i < sizeof(vidd_commands)/sizeof(struct command); i++)
				{
					if (!strcmp(command, vidd_commands[i].name))
					{
						command_found = true;
						vidd_commands[i].func(client, &command[strlen(command)+1]);
						break;
					}
				}

				cursor_restore();

				if (vidd_get_active() != client)
				{
					buffer_clear(&command_input);
					break;
				}

				client->mode = VIDD_MODE_NORMAL;
				vidd_set_status(client);

				if (!command_found)
				{
					intmax_t line;
					if (sscanf(command_input.data, "%ld", &line) >= 0)
					{
						vidd_move_to(client, client->cursor.x, line);
					}
				}
				buffer_clear(&command_input);
			}
			else
			{
				buffer_push(&command_input, key);
				printf("%c", key);
			}
		} break;
		case VIDD_MODE_FIND:
		case VIDD_MODE_FIND_REVERSE:
		{
			if (key == 27)
			{
				buffer_clear(&command_input);
				cursor_restore();
				client->mode = VIDD_MODE_NORMAL;
				vidd_set_status(client);
			}
			else if (key == 127)
			{
				if (command_input.length == 0) break;
				cursor_move(-1, 0);
				printf(" ");
				cursor_move(-1, 0);
				buffer_pop(&command_input);
			}
			else if (key == KEY_RETURN)
			{
				buffer_copy(&client->last_find, &command_input);
				char* word = command_input.data;

				cursor_restore();
				client->mode = VIDD_MODE_NORMAL;
				vidd_set_status(client);

				if (client->mode == VIDD_MODE_FIND)
					vidd_find_next_word(client, word, strlen(word));
				else vidd_find_prev_word(client, word, strlen(word));

				buffer_clear(&command_input);
			}
			else
			{
				buffer_push(&command_input, key);
				printf("%c", key);
			}
		} break;
		default: vidd_normal_mode_keybinds[key](client);
	}
}
void vidd_continue_input(struct vidd_client* client)
{
	uint32_t key = key = getch();
	vidd_interrupt(client, key);
}

void vidd_main(void)
{
	//dbs_start(0, "127.0.0.1");
	command_input = make_buffer(150);
	copy_buffer = make_buffer(2048);
	run_buffer = make_buffer(2048);
	while (1) vidd_continue_input(vidd_get_active());
}


void signal_catch(int ipar)
{

}

int main(int argc, char** argv)
{
	char* file_name = (argc > 1) ? (argv[1]) : ("_-=[NONE]=-_");
	//signal(SIGINT, signal_catch);
	screen_save();
	setbuf(stdout, NULL);

	client_pool = make_vidd_client_pool(20);

	intmax_t width, height;
	screen_get_size(&width, &height);
	struct vidd_client initial_client = make_vidd_client(file_name, 0, 0, width, height);

	vidd_client_pool_add(&client_pool, initial_client);

	if (!strcmp(file_name, "-"))
		vidd_load_stdin(vidd_get_active());
	else vidd_load_file(vidd_get_active(), file_name);
	getch_init();
	vidd_redraw(vidd_get_active());
	vidd_main();
	screen_restore();
	return 0;
}

