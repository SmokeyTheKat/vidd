#ifndef __VIDD_GETCH_H__
#define __VIDD_GETCH_H__

#include <stdint.h>
#include <stdbool.h>

uint32_t getch(bool raw);
void getch_exit(void);
void getch_init(void);

#endif