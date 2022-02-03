#ifndef __VIDD_ARGS_H__
#define __VIDD_ARGS_H__

#include <vidd/list.h>
#include <vidd/buffer.h>

#include <stddef.h>
#include <stdbool.h>

struct arguments;

struct arguments parse_args(int argc, char** argv);

struct arguments
{
	struct list files;
	bool read_stdin;
	bool readonly;
};

#endif