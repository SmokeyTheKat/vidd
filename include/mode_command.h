#ifndef __VIDD_MODE_COMMAND_H__
#define __VIDD_MODE_COMMAND_H__

#include "vidd.h"
#include <stdint.h>

void vidd_command_mode_interrupt(struct vidd_client* client, uint32_t key);
void vidd_command_mode_exit(struct vidd_client* client);
void vidd_command_mode_cycle_through_files(struct vidd_client* client);

#endif