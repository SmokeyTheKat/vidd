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

void vidd_fuzzy_find(struct vidd_client* client)
{
	intmax_t width, height;
	screen_get_size(&width, &height);

	struct list entries = make_list(100, struct filepath);
	get_files(&entries, ".");
	screen_clear();
	for (list_iterate_max(&entries, i, height, struct filepath))
		printf("%s\r\n", i->path.data);
	int pos = 0;
	struct buffer typed = make_buffer(100);
	while (1)
	{
		char key = getch(true);
		printf("\x1b[2J\x1b[1;1H");
		if (key == KEY_ESCAPE)
		{
			vidd_reorganize_clients(&client_pool);
			return;
		}
		else if (key == KEY_TAB)
		{
			pos++;
			int rpos = 0;
			for (list_iterate(&entries, i, struct filepath))
				if (strncmp(typed.data, i->name.data, typed.length) == 0)
					rpos++;
			if (pos >= rpos) pos = 0;
			
		}
		else if (key == KEY_RETURN)
		{
			vidd_reorganize_clients(&client_pool);
			int rpos = 0;
			for (list_iterate(&entries, i, struct filepath))
			{
				if (strncmp(typed.data, i->name.data, typed.length) == 0)
				{
					if (rpos == pos)
					{
						vidd_edit(client, i->path.data);
						vidd_reorganize_clients(&client_pool);
						return;
					}
					rpos++;
				}
			}
		}
		else if (key == KEY_BACKSPACE)
		{
			if (typed.length == 0)
			{
				vidd_reorganize_clients(&client_pool);
				return;
			}
			buffer_pop(&typed);
		}
		else buffer_push(&typed, key);
		int rpos = 0;
		printf("%s\r\n", typed.data);
		for (list_iterate_condition(&entries, i, rpos < height, struct filepath))
		{
			if (strncmp(typed.data, i->name.data, typed.length) == 0)
			{
				if (rpos == pos)
					printf("\x1b[7m%s\r\n", i->path.data);
				else printf("\x1b[0m%s\r\n", i->path.data);
				rpos++;
			}
		}
	}
}

#endif