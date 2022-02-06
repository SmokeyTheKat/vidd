#ifndef __VIDD_LAYOUTS_H__
#define __VIDD_LAYOUTS_H__

#include <vidd/tab.h>

#define LAYOUT_TILED 0

void vidd_arrange_clients(struct vidd_tab* tab);

extern void(*vidd_layouts[])(struct vidd_tab* tab);
extern int vidd_layouts_length;

#endif