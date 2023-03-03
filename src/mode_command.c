#include <vidd/mode_command.h>

#include <vidd/utils.h>
#include <vidd/config.h>
#include <vidd/getch.h>
#include <vidd/commands.h>

#include <dirent.h>

void vidd_command_mode_exit(struct vidd_client* client)
{
	buffer_clear(&command_input);
	cursor_restore();
	client->mode = VIDD_MODE_NORMAL;
	vidd_set_status(client);
}

void vidd_command_mode_cycle_through_files(struct vidd_client* client)
{
		int skip = 0;
		char curdir_carray[1024] = {0};
		char* curdir = curdir_carray;
		strcpy(curdir, strchr(command_input.data, ' ') + 1);
		int curdir_len = strlen(curdir);
		if (*(uint16_t*)curdir == *(uint16_t*)"./")
		{
			curdir_len -= 2;
			curdir = curdir + 2;
		}
		DIR* d;
		struct dirent* dir;
		uint32_t key = 0;
		char finddir_carray[1024] = {0};
		char* finddir = finddir_carray;
		char* findlslash = strrchr(curdir, '/');
		if (!findlslash) finddir = ".";
		else
		{
			memcpy(finddir, curdir, curdir_len - strlen(findlslash));
			finddir[curdir_len - curdir_len - strlen(findlslash)] = 0;
			skip = curdir_len - strlen(findlslash) + 1;
			curdir = findlslash + 1;
		}
TAB_FETCH_REDO:
		d = opendir(finddir);
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
			{
				if (!strncmp(curdir, dir->d_name, strlen(curdir)))
				{
					char* mcpos = strchr(command_input.data, ' ') + 1 + skip;
					int mcpos_len = strlen(mcpos);
					buffer_delete_n_at(&command_input, mcpos_len,
							   (uint64_t)mcpos - (uint64_t)command_input.data);
					buffer_push_cstring(&command_input, dir->d_name, strlen(dir->d_name));
					cursor_move(-mcpos_len, 0);
					for (int i = 0; i < mcpos_len; i++)
						printf(" ");
					cursor_move(-mcpos_len, 0);
					printf("%s", dir->d_name);
					key = getch(false);
					if (key != '\t')
					{
						vidd_command_mode_interrupt(client, key);
						return;
					}
				}
			}
			closedir(d);
		}
		if (key == '\t') goto TAB_FETCH_REDO;
}

void vidd_command_mode_interrupt(struct vidd_client* client, uint32_t key)
{
	if (key == 27) vidd_command_mode_exit(client);
	else if (key == KEY_TAB)
	{
		vidd_command_mode_cycle_through_files(client);
	}
	else if (key == KEY_BACKSPACE)
	{
		if (command_input.length == 0)
		{
			vidd_command_mode_exit(client);
			return;
		}
		cursor_move(-1, 0);
		printf(" ");
		cursor_move(-1, 0);
		buffer_pop(&command_input);
	}
	else if (key == KEY_RETURN)
	{
		client->mode = VIDD_MODE_NORMAL;
		bool command_found = false;
		if (command_input.length > 0)
		{
			char* command = strtok(command_input.data, " \n\t\0 ");
			for (intmax_t i = 0; i < vidd_commands_length; i++)
			{
				if (!strcmp(command, vidd_commands[i].name))
				{
					command_found = true;
					vidd_commands[i].func(client, &command[strlen(command)+1]);
					break;
				}
			}
		}
		
		cursor_restore();
		
		if (vidd_get_active() != client)
		{
			buffer_clear(&command_input);
			return;
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
}
