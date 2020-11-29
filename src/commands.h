#ifndef __vidd_commands_h__
#define __vidd_commands_h__

#include <ddcCursor.h>

#include "./vidd.h"

void vidd_command_q(struct vidd* vidd)
{
	cursor_clear();
	exit(0);
}
void vidd_command_w(struct vidd* vidd)
{
	(void)vidd;
}

const sizet viddCommandVectorLength = 2;
const struct viddCommand viddCommandVector[] = {
	{ "q", vidd_command_q },
	{ "w", vidd_command_w }
};

#endif
