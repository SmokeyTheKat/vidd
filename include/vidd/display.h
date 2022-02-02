#ifndef __VIDD_DISPLAY_H__
#define __VIDD_DISPLAY_H__

#include "vidd.h"

void vidd_redraw(struct vidd_client* client);
void vidd_set_status(struct vidd_client* client);
void vidd_show_message(struct vidd_client* client, char* message);
void vidd_show_error(struct vidd_client* client, char* message);

#endif