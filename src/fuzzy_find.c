#include <vidd/fuzzy_find.h>

#include <vidd/list.h>
#include <vidd/tab.h>
#include <vidd/layouts.h>
#include <vidd/commands.h>
#include <vidd/style.h>
#include <vidd/themes.h>
#include <vidd/vidd.h>
#include <vidd/getch.h>

#include <string.h>
#include <dirent.h>

static void fuzzy_find_clear_area(void);
static void fuzzy_find_draw_border(char* title);
static void fuzzy_find_clear_area(void);
static void fuzzy_find_draw_border(char* title);
static void fuzzy_find_display_data(struct list* entries, struct buffer* tofind, int cursor);
static char* fuzzy_find_get_path_at_cursor(struct list* entries, struct buffer* tofind, int cursor);
static void fuzzy_find_close(struct list* entries);

static int width = 0;
static int height = 0;
static int top = 0;
static int left = 0;

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
	vidd_fuzzy_find(client, "open", vidd_open);
}

void vidd_fuzzy_find_edit(struct vidd_client* client)
{
	vidd_fuzzy_find(client, "edit", vidd_edit);
}

void vidd_fuzzy_find_float(struct vidd_client* client)
{
	vidd_fuzzy_find(client, "float", vidd_open_in_floating_window);
}

static void fuzzy_find_clear_area(void)
{
	struct buffer toprint = make_buffer(width * height);

	for (intmax_t y = top + 1; y < top + height; y++)
	{
		buffer_printf(&toprint, CURSOR_TO("%d", "%d"), y + 1, left + 1 + 1);
		buffer_push_repeat(&toprint, ' ', width - 1);
	}

	printf("%s", toprint.data);

	free_buffer(&toprint);
}

static void fuzzy_find_draw_border(char* title)
{
	printf("%s", active_theme->bg_style);
	printf("%s", active_theme->fg_style);

	cursor_move_to(left, top);
	printf("┌" CURSOR_DOWN() CURSOR_LEFT());
	for (intmax_t i = 0; i < height; i++)
		printf("│" CURSOR_DOWN() CURSOR_LEFT());

	cursor_move_to(left + width, top);
	printf("┐" CURSOR_LEFT() CURSOR_DOWN());
	for (intmax_t i = 1; i < height; i++)
		printf("│" CURSOR_DOWN() CURSOR_LEFT());
	printf("┘");

	cursor_move_to(left, top + height);
	printf("└");
	for (intmax_t i = 1; i < width; i++)
		printf("─");

	cursor_move_to(left + 1, top);
	for (intmax_t i = 1; i < width; i++)
		printf("─");

	cursor_move_to(left + (width / 2) - ((strlen(title) + 2) / 2), top);
	printf(" %s ", title);
}

static void fuzzy_find_display_data(struct list* entries, struct buffer* tofind, int cursor)
{
	fuzzy_find_clear_area();

	struct buffer toprint = make_buffer(width * height);

	int y = 1;
	buffer_printf(&toprint, CURSOR_TO("%d", "%d"), top + y++ + 1, left + 1 + 1);
	buffer_print(&toprint, tofind->data);

	int pos = 0;
	for (list_iterate(entries, i, struct filepath))
	{
		if (!string_includes_list(i->path.data, tofind->data)) continue;

		buffer_printf(&toprint, CURSOR_TO("%d", "%d"), top + y + 1, left + 1 + 1);

		if (cursor == pos)
			buffer_print(&toprint, active_theme->highlight_style);

		buffer_print(&toprint, i->path.data);

		buffer_print(&toprint, NOSTYLE);
		buffer_print(&toprint, active_theme->bg_style);
		buffer_print(&toprint, active_theme->fg_style);

		y++;
		pos++;
		if (y == height) break;
	}

	printf("%s", toprint.data);

	free_buffer(&toprint);
}

static char* fuzzy_find_get_path_at_cursor(struct list* entries, struct buffer* tofind, int cursor)
{
	int pos = 0;
	for (list_iterate(entries, i, struct filepath))
	{
		if (!string_includes_list(i->path.data, tofind->data)) continue;
		if (pos == cursor)
			return i->path.data;
		pos++;
	}
	return 0;
}

static void fuzzy_find_close(struct list* entries)
{
	for (list_iterate(entries, i, struct filepath))
	{
		free_buffer(&i->path);
		free_buffer(&i->name);
	}
	free_list(entries);
	vidd_arrange_clients(active_tab);
}

void vidd_fuzzy_find(struct vidd_client* client, char* title, void(*out_function)(struct vidd_client*, char*))
{
	intmax_t screen_width, screen_height;
	screen_get_size(&screen_width, &screen_height);

	struct list entries = make_list(100, struct filepath);
	get_files(&entries, ".");

	width = screen_width * 0.8;
	height = screen_height * 0.8;
	top = screen_height * 0.1;
	left = screen_width * 0.1;

	fuzzy_find_draw_border(title);

	int cursor = 0;
	struct buffer typed = make_buffer(100);

	fuzzy_find_display_data(&entries, &typed, cursor);

	while (1)
	{
		char key = getch(true);
		switch (key)
		{
			case KEY_ESCAPE:
			{
				fuzzy_find_close(&entries);
				return;
			} break;
			case KEY_TAB:
			{
				cursor++;
			} break;
			case KEY_RETURN:
			{
				char* path = fuzzy_find_get_path_at_cursor(&entries, &typed, cursor);
				if (path != 0)
					out_function(client, path);
				fuzzy_find_close(&entries);
				return;
			} break;
			case KEY_BACKSPACE:
			{
				if (typed.length == 0)
				{
					fuzzy_find_close(&entries);
					return;
				}
				buffer_pop(&typed);
				cursor = 0;
			} break;
			default:
			{
				buffer_push(&typed, key);
				cursor = 0;
			} break;
		}
		fuzzy_find_display_data(&entries, &typed, cursor);
	}

	fuzzy_find_close(&entries);
}
