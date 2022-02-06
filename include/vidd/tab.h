#ifndef __VIDD_TAB_H__
#define __VIDD_TAB_H__

#include <vidd/vidd.h>

struct vidd_tab;

void init_vidd_tab(void);
struct vidd_tab make_vidd_tab(void);
void vidd_tab_next_layout(struct vidd_tab* tab);
struct vidd_client* vidd_tab_add_client(struct vidd_tab* tab, struct vidd_client client);
void vidd_tab_remove_client(struct vidd_tab* tab, struct vidd_client* client);
void vidd_tab_set(int tab);
int vidd_tab_get_client_index(struct vidd_tab* tab, struct vidd_client* client);
void vidd_tab_set_active(struct vidd_tab* tab, struct vidd_client* client);
struct vidd_client* vidd_tab_get_active(struct vidd_tab* tab);
int vidd_tab_get_non_floating_count(struct vidd_tab* tab);
void vidd_tab_next_client(struct vidd_tab* tab);
void vidd_tab_prev_client(struct vidd_tab* tab);

struct vidd_tab
{
	int active;
	struct vidd_client* clients;
	int length;
	int capacity;
	float master_size;
	int master_count;
	int layout;
};

extern struct vidd_tab* active_tab;

#endif