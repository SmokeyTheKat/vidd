#ifndef __VIDD_LINE_H__
#define __VIDD_LINE_H__

#include "./buffer.h"

#include <stdbool.h>

#define DEFAULT_LINE_LENGTH 150

struct line;

struct line* new_line(struct line* line);
#define line_insert new_line
struct line* new_line_dont_adjust_line_number(struct line* line);
#define line_insert_dont_adjust_line_number new_line_dont_adjust_line_number
struct line* free_line(struct line* line);
#define line_remove free_line

struct line* line_get_line(struct line* line, intmax_t y);
struct line* line_get_first(struct line* line);
struct line* line_get_last(struct line* line);
struct line* line_get_line_in_direction(struct line* line, int dy);
struct line* line_skip(struct line* line, intmax_t count);

bool line_is_empty(struct line* line);

void line_set_text(struct line* line, char* text, intmax_t length);
void line_insert_at(struct line* line, char chr, intmax_t pos);
void line_append(struct line* line, char chr);

void line_delete_n_at(struct line* line, intmax_t n, intmax_t pos);
void line_delete_at(struct line* line, intmax_t pos);
void line_delete_after(struct line* line, intmax_t pos);
void line_delete_before(struct line* line, intmax_t pos);

void line_split_at(struct line* line, intmax_t pos);
void line_join_next(struct line* line);

void line_join_line_strings(struct line* dest, struct line* src);

void line_adjust_all_numbers(struct line* line);

struct line
{
	struct buffer buffer;
	struct line* next;
	struct line* prev;
	intmax_t number;
};

#endif