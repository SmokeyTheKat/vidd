#ifndef __VIDD_FUZZY_FIND_H__
#define __VIDD_FUZZY_FIND_H__

#include <string.h>
#include <dirent.h>

#include "list.h"

struct filepath
{
	struct buffer name;
	struct buffer path;
};

void get_files(struct list* list, char* base_path);
void vidd_fuzzy_find(struct vidd_client* client, void(*out_function)(struct vidd_client*, char*));
void vidd_fuzzy_find_open(struct vidd_client* client);
void vidd_fuzzy_find_vsplit(struct vidd_client* client);
void vidd_fuzzy_find_float(struct vidd_client* client);

void get_files(struct list* list, char* base_path)
{
	char path[1024] = {0};
	struct dirent* dp;
	DIR* dir = opendir(base_path);

	if (!dir) return;

	while ((dp = readdir(dir)) != 0)
	{
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
		{
			strcpy(path, base_path);
			strcat(path, "/");
			strcat(path, dp->d_name);

			list_push(list, ((struct filepath){make_buffer_from_cstring(dp->d_name), make_buffer_from_cstring(path)}), struct filepath);

			get_files(list, path);
		}
	}
	closedir(dir);
}

void vidd_fuzzy_find_open(struct vidd_client* client)
{
	vidd_fuzzy_find(client, vidd_edit);
}

void vidd_fuzzy_find_vsplit(struct vidd_client* client)
{
	vidd_fuzzy_find(client, vidd_vsplit);
}

void vidd_fuzzy_find_float(struct vidd_client* client)
{
	vidd_fuzzy_find(client, vidd_open_in_floating_window);
}

void vidd_fuzzy_find(struct vidd_client* client, void(*out_function)(struct vidd_client*, char*))
{
	intmax_t width, height;
	screen_get_size(&width, &height);
	height--;

	struct list entries = make_list(100, struct filepath);
	get_files(&entries, ".");
	screen_clear();
	for (list_iterate_max(&entries, i, height, struct filepath))
		printf("%s\r\n", i->path.data+2);
	int pos = 0;
	struct buffer typed = make_buffer(100);
	while (1)
	{
		char key = getch(true);
		printf("\x1b[2J\x1b[1;1H");
		if (key == KEY_ESCAPE)
		{
			goto VIDD_FUZZY_FIND_EXIT;
		}
		else if (key == KEY_TAB)
		{
			pos++;
			int rpos = 0;
			for (list_iterate(&entries, i, struct filepath))
				if (strstr(i->path.data, typed.data)) rpos++;
			if (pos >= rpos) pos = 0;
		}
		else if (key == KEY_RETURN)
		{
			vidd_reorganize_clients(&client_pool);
			int rpos = 0;
			for (list_iterate(&entries, i, struct filepath))
			{
				if (strstr(i->path.data, typed.data))
				{
					if (rpos == pos)
					{
						out_function(client, i->path.data);
						goto VIDD_FUZZY_FIND_EXIT;
					}
					rpos++;
				}
			}
		}
		else if (key == KEY_BACKSPACE)
		{
			if (typed.length == 0)
				goto VIDD_FUZZY_FIND_EXIT;
			buffer_pop(&typed);
		}
		else buffer_push(&typed, key);
		int rpos = 0;
		printf("\x1b[0m%s\r\n", typed.data);
		for (list_iterate_condition(&entries, i, rpos < height-1, struct filepath))
		{
			char* find_pos = 0;
			if ((find_pos = strstr(i->path.data, typed.data)))
			{
				if (rpos == pos)
					printf("\x1b[0m\x1b[38;2;255;255;0m\x1b[7m%s\r\n", i->path.data+2);
				else printf("\x1b[0m%s\x1b[%dD\x1b[38;2;255;255;0m%s\r\n", i->path.data+2,
							(int)(((ptrdiff_t)&i->path.data[i->path.length]) - ((ptrdiff_t)find_pos)),
							typed.data);
				rpos++;
			}
		}
	}
VIDD_FUZZY_FIND_EXIT:
	vidd_reorganize_clients(&client_pool);
	for (list_iterate(&entries, i, struct filepath))
	{
		free_buffer(&i->path);
		free_buffer(&i->name);
	}
}

#endif