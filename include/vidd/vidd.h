#ifndef __VIDD_H__
#define __VIDD_H__

#include "line.h"
#include "buffer.h"
#include "list.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

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
char vidd_is_real_file(struct vidd_client* client);
void vidd_continue_input(struct vidd_client* client);
struct vidd_client make_vidd_client(char* file_name, intmax_t x, intmax_t y, intmax_t width, intmax_t height, int* open_buffers);
void free_vidd_client(struct vidd_client* client);
void vidd_load_from_fp(struct line* line, FILE* fp);
void vidd_load_file(struct vidd_client* client, char* file_name);
void vidd_load_stdin(struct vidd_client* client);
struct vidd_client_pool make_vidd_client_pool(intmax_t start_size);
struct vidd_client* vidd_client_pool_add(struct vidd_client_pool* pool, struct vidd_client client);
intmax_t vidd_client_pool_get_client_index(struct vidd_client_pool* pool, struct vidd_client* client);
void vidd_client_pool_set_active(struct vidd_client_pool* pool, struct vidd_client* client);
void vidd_client_pool_next_client(struct vidd_client_pool* pool);
void vidd_client_pool_prev_client(struct vidd_client_pool* pool);
void vidd_set_mode_texts_names(void);

enum
{
	VIDD_MODE_NORMAL=0,
	VIDD_MODE_COMMAND,
	VIDD_MODE_WINDOW_MOVE,
	VIDD_MODE_FIND,
	VIDD_MODE_FIND_REVERSE,
	VIDD_MODE_INSERT,
	VIDD_MODE_REPLACE,
	VIDD_MODE_SELECT,
	VIDD_MODE_LINE_SELECT,
};

enum
{
	VIDD_ACTION_MOVEMENT=0,
	VIDD_ACTION_EDIT,
};

struct command
{
	char* name;
	void(*func)(struct vidd_client*, char*);
};

struct vidd_keybind
{
	void(*func)(struct vidd_client*);
	int type;
};

struct vidd_client
{
	struct buffer file_name;
	intmax_t x, y;
	intmax_t width, height;
	uint32_t key;
	int* open_buffers;
	struct line* text;

	struct viewport
	{
		intmax_t x, y;
		intmax_t xo, yo;
		intmax_t width, height;
	} view;

	struct cursor
	{
		intmax_t x;
		intmax_t lx;
		struct line* y;
	} cursor;

	struct marker
	{
		struct
		{
			intmax_t x, y;
		} positions[510];
	} marker;

	struct selection
	{
		intmax_t x0, y0;
		intmax_t x1, y1;
	} selection;

	struct buffer make_command;
	struct buffer last_find;
	void(**keybinds)(struct vidd_client*);
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
			intmax_t outputTabs : 1;
			intmax_t readOnly : 1;
		};
	};
	union
	{
		intmax_t status;
		struct
		{
			intmax_t unsavedChanges : 1;
			intmax_t isFloating : 1;
		};
	};
	int mode;
};

struct vidd_client_pool
{
	intmax_t active;
	struct vidd_client* clients;
	float master_size;
	intmax_t length;
	intmax_t size;
};
 
extern struct vidd_client_pool client_pool;

extern struct buffer command_input;
extern struct buffer copy_buffer;
extern struct buffer run_buffer;
extern struct buffer macro_buffer;

extern bool macro_recording;

extern char* vidd_mode_texts[];
extern int vidd_mode_texts_length;

#endif