#include <vidd/tab.h>

#include <vidd/vidd.h>
#include <vidd/config.h>
#include <vidd/layouts.h>

#define MAX_CLIENTS 20

static struct vidd_tab tabs[] = {
	[0 ... 9]={0},
};
static int tabs_length = sizeof(tabs) / sizeof(tabs[0]);

struct vidd_tab* active_tab = &tabs[0];

void init_vidd_tab(void)
{
	for (int i = 0; i < tabs_length; i++)
		tabs[i] = make_vidd_tab();
}

struct vidd_tab make_vidd_tab(void)
{
	struct vidd_tab tab = { 0 };
	tab.length = 0;
	tab.capacity = MAX_CLIENTS;
	tab.clients = malloc(sizeof(struct vidd_client) * tab.capacity);
	tab.active = 0;
	tab.layout = LAYOUT_TILED;
	tab.master_size = 0.5;
	tab.master_count = 1;
	return tab;
}

void vidd_tab_next_layout(struct vidd_tab* tab)
{
	tab->layout++;
	if (tab->layout == vidd_layouts_length)
		tab->layout = 0;
	vidd_arrange_clients(tab);
}

struct vidd_client* vidd_tab_add_client(struct vidd_tab* tab, struct vidd_client client)
{
	if (tab->length + 1 >= tab->capacity)
		tab->clients = realloc(tab->clients, ++tab->capacity);

	client.tab = tab;

	tab->clients[tab->length] = client;
	return &tab->clients[tab->length++];
}

void vidd_tab_remove_client(struct vidd_tab* tab, struct vidd_client* client)
{
	intmax_t pos = vidd_tab_get_client_index(tab, client);
	free_vidd_client(client);
	for (intmax_t i = pos; i < tab->length - 1; i++)
		tab->clients[i] = tab->clients[i+1];
	tab->length--;
	tab->clients[tab->length] = (struct vidd_client){ 0 };
	tab->active = 0;
}

void vidd_tab_set(int tab)
{
	active_tab = &tabs[tab - 1];

	if (active_tab->length == 0)
	{
		struct vidd_client client = make_vidd_client(DEFAULT_FILE, 0, 0, 0, 0, 0);
		vidd_load_file(&client, DEFAULT_FILE);
		vidd_tab_add_client(active_tab, client);
	}
	vidd_arrange_clients(active_tab);
}

int vidd_tab_get_client_index(struct vidd_tab* tab, struct vidd_client* client)
{
	for (intmax_t i = 0; i < tab->length; i++)
		if (client == &tab->clients[i]) return i;
	return -1;
}

void vidd_tab_set_active(struct vidd_tab* tab, struct vidd_client* client)
{
	tab->active = vidd_tab_get_client_index(tab, client);
}

struct vidd_client* vidd_tab_get_active(struct vidd_tab* tab)
{
	return &tab->clients[tab->active];
}

int vidd_tab_get_non_floating_count(struct vidd_tab* tab)
{
	int count = 0;
	for (int i = 0; i < tab->length; i++)
		if (!tab->clients[i].isFloating)
			count++;
	return count;
}

void vidd_tab_next_client(struct vidd_tab* tab)
{
	tab->active++;
	if (tab->active >= tab->length) tab->active = 0;
}

void vidd_tab_prev_client(struct vidd_tab* tab)
{
	tab->active--;
	if (tab->active < 0) tab->active = tab->length-1;
}

