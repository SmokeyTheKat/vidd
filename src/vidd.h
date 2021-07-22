#ifndef __VIDD_H__
#define __VIDD_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include "./line.h"
#include "./list.h"
#include "./utils.h"


#define CSTR_REMOVE_NL(b, l) { \
		if (b[l-1] == '\n') \
		{ \
			b[l-1] = 0; \
			l--; \
		} \
	}

struct cursor;
struct viewport;
struct command;
struct vidd_client;
struct vidd_client_pool;

void vidd_main(void);
struct vidd_client* vidd_get_active(void);
void vidd_interrupt(struct vidd_client* client, uint32_t key);
void vidd_continue_input(struct vidd_client* client);
struct vidd_client make_vidd_client(char* file_name, intmax_t x, intmax_t y, intmax_t width, intmax_t height);
void free_vidd_client(struct vidd_client* client);
void vidd_load_file(struct vidd_client* client, char* file_name);
void vidd_load_stdin(struct vidd_client* client);
struct vidd_client* vidd_client_pool_add(struct vidd_client_pool* pool, struct vidd_client client);
intmax_t vidd_client_pool_get_client_index(struct vidd_client_pool* pool, struct vidd_client* client);
void vidd_client_pool_set_active(struct vidd_client_pool* pool, struct vidd_client* client);
void vidd_client_pool_next_client(struct vidd_client_pool* pool);
void vidd_client_pool_prev_client(struct vidd_client_pool* pool);

enum
{
	VIDD_MODE_NORMAL=0,
	VIDD_MODE_COMMAND,
	VIDD_MODE_FIND,
	VIDD_MODE_INSERT,
	VIDD_MODE_REPLACE,
	VIDD_MODE_SELECT,
	VIDD_MODE_LINE_SELECT,
};

const char* VIDD_MODE_TEXTS[] = {
	[VIDD_MODE_NORMAL]="[NORMAL]",
	[VIDD_MODE_COMMAND]="[COMMAND]",
	[VIDD_MODE_INSERT]="[INSERT]",
	[VIDD_MODE_REPLACE]="[REPLACE]",
	[VIDD_MODE_SELECT]="[SELECT]",
	[VIDD_MODE_LINE_SELECT]="[LINE SELECT]",
};

struct command
{
	char* name;
	void(*func)(struct vidd_client*, char*);
};

struct cursor
{
	intmax_t x;
	intmax_t lx;
	struct line* y;
};

struct viewport
{
	intmax_t x, y;
	intmax_t xo, yo;
	intmax_t width, height;
};

struct selection
{
	intmax_t x0, y0;
	intmax_t x1, y1;
};

struct marker
{
	struct
	{
		intmax_t x, y;
	} positions[510];
};

struct vidd_client
{
	char* file_name;
	intmax_t x, y;
	intmax_t width, height;
	uint32_t key;

	struct line* text;
	struct cursor cursor;
	struct viewport view;
	struct marker marker;
	struct selection selection;
	char** syntax;


	union
	{
		intmax_t settings;
		struct
		{
			intmax_t numbersOn : 1;
			intmax_t displayOn : 1;
			intmax_t syntaxOn : 1;
			intmax_t inclusiveSelection : 1;
		};
	};
	int mode;
};

struct vidd_client_pool
{
	intmax_t active;
	struct vidd_client* clients;
	intmax_t length;
	intmax_t size;
};

struct vidd_client_pool client_pool;
struct buffer command_input;
struct buffer copy_buffer;
struct buffer run_buffer;

uint32_t getch(void);
struct vidd_client* vidd_get_active(void);
intmax_t vidd_client_pool_get_client_index(struct vidd_client_pool* pool, struct vidd_client* client);
void vidd_client_pool_set_active(struct vidd_client_pool* pool, struct vidd_client* client);
void vidd_client_pool_prev_client(struct vidd_client_pool* pool);
void vidd_client_pool_next_client(struct vidd_client_pool* pool);
struct vidd_client_pool make_vidd_client_pool(intmax_t start_size);
struct vidd_client* vidd_client_pool_add(struct vidd_client_pool* pool, struct vidd_client client);
struct vidd_client make_vidd_client(char* file_name, intmax_t x, intmax_t y, intmax_t width, intmax_t height);
void free_vidd_client(struct vidd_client* client);
void vidd_load_stdin(struct vidd_client* client);
void vidd_load_file(struct vidd_client* client, char* file_name);
void vidd_insert_char(struct vidd_client* client);
void vidd_interrupt(struct vidd_client* client, uint32_t key);
void vidd_continue_input(struct vidd_client* client);
void vidd_main(void);
void signal_catch(int ipar);

#endif
