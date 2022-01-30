#ifndef __VIDD_SYNTAX_H__
#define __VIDD_SYNTAX_H__

#include "vidd.h"
#include "line.h"
#include "buffer.h"

void vidd_syntax_apply_to_buffer(struct vidd_client* client, struct buffer* buffer, struct line* line);
void vidd_syntax_ftdetect(struct vidd_client* client);
void vidd_syntax_ftdetect_cstring(struct vidd_client* client, char* rfile_name);

#endif