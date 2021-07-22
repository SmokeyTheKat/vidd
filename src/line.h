#ifndef __VIDD_LINE_H__
#define __VIDD_LINE_H__

#include <stdlib.h>
#include <string.h>

#include "./buffer.h"

#define DEFAULT_LINE_LENGTH 150

struct line;

struct line* new_line(struct line* line);
#define line_insert new_line
struct line* free_line(struct line* line);
#define line_remove free_line

struct line* line_get_line(struct line* line, intmax_t y);
struct line* line_get_first(struct line* line);
struct line* line_get_last(struct line* line);
struct line* line_get_line_in_direction(struct line* line, int dy);
struct line* line_skip(struct line* line, intmax_t count);


void line_set_text(struct line* line, char* text, intmax_t length);
void line_insert_at(struct line* line, char chr, intmax_t pos);
void line_append(struct line* line, char chr);

void line_delete_n_at(struct line* line, intmax_t n, intmax_t pos);
void line_delete_at(struct line* line, intmax_t pos);
void line_delete_after(struct line* line, intmax_t pos);
void line_delete_before(struct line* line, intmax_t pos);

void line_split_at(struct line* line, intmax_t pos);
void line_join_next(struct line* line);

void line_adjust_all_numbers(struct line* line);

struct line
{
	struct buffer buffer;
	struct line* next;
	struct line* prev;
	intmax_t number;
};

struct line* new_line(struct line* line)
{
	struct line* new_line = malloc(sizeof(struct line));
	new_line->buffer = make_buffer(DEFAULT_LINE_LENGTH);

	if (line)
	{
		if (line->next)
		{
			new_line->next = line->next;
			line->next->prev = new_line;
		}
		else
		{
			new_line->next = 0;
		}
		line->next = new_line;
		new_line->prev = line;
		new_line->number = line->number + 1;
	}
	else
	{
		new_line->next = 0;
		new_line->prev = 0;
		new_line->number = 1;
	}

	line_adjust_all_numbers(new_line);

	return new_line;
}


struct line* free_line(struct line* line)
{
	if (line == 0) return 0;
	struct line* line_out = line;
	if (line->prev)
	{
		line->prev->next = line->next;
		line_out = line->prev;
	}
	if (line->next)
	{
		line->next->prev = line->prev;
		line_out = line->next;
	}
	line_adjust_all_numbers(line_out);
	if (line_out != line)
	{
		free_buffer(&line->buffer);
		if (line) free(line);
	}
	return line_out;
}

struct line* line_get_line(struct line* line, intmax_t y)
{
	if (y == line->number) return line;
	else if (y > line->number)
		while (line->next && line->number != y) line = line->next;
	else if (y < line->number)
		while (line->prev && line->number != y) line = line->prev;
	return line;
}

struct line* line_get_first(struct line* line)
{
	while (line->prev) line = line->prev;
	return line;
}

struct line* line_get_last(struct line* line)
{
	while (line->next) line = line->next;
	return line;
}

struct line* line_get_line_in_direction(struct line* line, int dy)
{
	if (dy > 0) return line->next;
	if (dy < 0) return line->prev;
	return 0;
}

struct line* line_skip(struct line* line, intmax_t count)
{
	for (intmax_t i = 0; line->next && i < count; i++)
		line = line->next;
	return line;
}

void line_set_text(struct line* line, char* text, intmax_t length)
{
	buffer_set_data(&line->buffer, text, length);
}

void line_insert_at(struct line* line, char chr, intmax_t pos)
{
	buffer_insert_at(&line->buffer, chr, pos);
}

void line_append(struct line* line, char chr)
{
	buffer_push(&line->buffer, chr);
}

void line_delete_n_at(struct line* line, intmax_t n, intmax_t pos)
{
	buffer_delete_n_at(&line->buffer, n, pos);
}

void line_delete_at(struct line* line, intmax_t pos)
{
	buffer_delete_at(&line->buffer, pos);
}

void line_delete_after(struct line* line, intmax_t pos)
{
	buffer_delete_after(&line->buffer, pos);
}

void line_delete_before(struct line* line, intmax_t pos)
{
	buffer_delete_before(&line->buffer, pos);
}

void line_split_at(struct line* line, intmax_t pos)
{
	struct line* line_new = line_insert(line);
	line_set_text(line_new, &line->buffer.data[pos], line->buffer.length - pos);
	memset(&line->buffer.data[pos], 0, line->buffer.length - pos);
	line->buffer.length -= line->buffer.length - pos;
}

void line_join_next(struct line* line)
{
	if (!line->next) return;
	buffer_push_cstring(&line->buffer, line->next->buffer.data, line->next->buffer.length);
	line_remove(line->next);
}

void line_adjust_all_numbers(struct line* line)
{
	line = line_get_first(line);
	intmax_t count = 1;
	while (line)
	{
		line->number = count++;
		line = line->next;
	}
}

#endif
