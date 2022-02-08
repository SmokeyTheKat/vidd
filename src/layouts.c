#include <vidd/layouts.h>

#include <vidd/display.h>
#include <vidd/vidd.h>
#include <vidd/tab.h>
#include <vidd/commands.h>

static void redraw_floating(struct vidd_tab* tab);
static void layout_tiled(struct vidd_tab* tab);
static void layout_monocle(struct vidd_tab* tab);

void(*vidd_layouts[])(struct vidd_tab* tab) = {
	layout_tiled,
	layout_monocle,
};
int vidd_layouts_length = sizeof(vidd_layouts) / sizeof(vidd_layouts[0]);

void vidd_arrange_clients(struct vidd_tab* tab)
{
	screen_clear();
	vidd_layouts[tab->layout](tab);
	redraw_floating(tab);
	vidd_cursor_adjust(vidd_get_active(), true);
	vidd_redraw(&tab->clients[tab->active]);
}

static void redraw_floating(struct vidd_tab* tab)
{
	for (int i = 0; i < tab->length; i++)
	{
		struct vidd_client* cur = &tab->clients[i];
		if (cur->isFloating)
			vidd_redraw(cur);
	}
}

static void layout_tiled(struct vidd_tab* tab)
{
	intmax_t screen_width, screen_height;
	screen_get_size(&screen_width, &screen_height);

	int client_count = vidd_tab_get_non_floating_count(tab);

	intmax_t master_count = MIN(client_count, tab->master_count);
	intmax_t stack_count = MAX(0, client_count - master_count);

	int new_width = (stack_count > 0) ? (screen_width * tab->master_size) : screen_width;
	int new_height = (master_count > 0) ? (screen_height / master_count) : 0;
	int extra_height = screen_height - new_height * master_count;
	int new_x = 0;
	int new_y = 0;

	for (int i = 0, j = 0; j < master_count; i++)
	{
		struct vidd_client* cur = &tab->clients[i];
		if (cur->isFloating) continue;
		cur->x = new_x;
		cur->y = new_y;
		cur->width = new_width;
		cur->height = new_height;
		new_y += (extra_height-- > 0) ? (++cur->height) : (cur->height);
		vidd_redraw(cur);
		vidd_draw_vsplit_line(cur);
		j++;
	}

	if (stack_count == 0)
	{
		redraw_floating(tab);
		return;
	}

	new_width = (master_count > 0) ? (screen_width - screen_width * tab->master_size - 1) : (screen_width);
	new_height = screen_height / stack_count;
	extra_height = screen_height - new_height * stack_count;
	new_x = (master_count > 0) ? (screen_width * tab->master_size + 1) : 0;
	new_y = 0;

	for (int i = 0, j = 0; j < client_count; i++)
	{
		struct vidd_client* cur = &tab->clients[i];
		if (cur->isFloating) continue;
		if (j++ < master_count) continue;
		cur->x = new_x;
		cur->y = new_y;
		cur->width = new_width;
		cur->height = new_height;
		new_y += (extra_height-- > 0) ? (++cur->height) : (cur->height);
		vidd_redraw(cur);
	}
}

static void layout_monocle(struct vidd_tab* tab)
{
	intmax_t screen_width, screen_height;
	screen_get_size(&screen_width, &screen_height);

	for (int i = 0; i < tab->length; i++)
	{
		struct vidd_client* cur = &tab->clients[i];
		if (cur->isFloating) continue;
		cur->x = 0;
		cur->y = 0;
		cur->width = screen_width;
		cur->height = screen_height;
		vidd_redraw(cur);
	}
}
