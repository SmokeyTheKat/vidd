#ifndef __VIDD_FUZZY_FIND_H__
#define __VIDD_FUZZY_FIND_H__

#include "list.h"
#include "vidd.h"

struct filepath;

void get_files(struct list* list, char* base_path);
void vidd_fuzzy_find(struct vidd_client* client, void(*out_function)(struct vidd_client*, char*));
void vidd_fuzzy_find_open(struct vidd_client* client);
void vidd_fuzzy_find_vsplit(struct vidd_client* client);
void vidd_fuzzy_find_float(struct vidd_client* client);

struct filepath
{
	struct buffer name;
	struct buffer path;
};

#endif