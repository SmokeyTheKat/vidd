#ifndef __VIDD_BUFFER_H__
#define __VIDD_BUFFER_H__

#include <stdint.h>

#define BUFFER_SPACER 80

struct buffer;

struct buffer make_buffer(intmax_t capacity);
struct buffer make_buffer_format(char* fmt, ...);
struct buffer make_buffer_from_cstring(char* cstr);
void free_buffer(struct buffer* buffer);

void buffer_copy(struct buffer* dest, struct buffer* src);
void buffer_copy_inverse(struct buffer* dest, struct buffer* src);

void buffer_printf(struct buffer* buffer, char* fmt, ...);
void buffer_print(struct buffer* buffer, char* data);
void buffer_insert_at(struct buffer* buffer, char value, intmax_t pos);
void buffer_set_data(struct buffer* buffer, char* data, intmax_t length);

void buffer_clear(struct buffer* buffer);

void buffer_push(struct buffer* buffer, char value);
void buffer_push_repeat(struct buffer* buffer, char value, intmax_t rept);
void buffer_push_cstring(struct buffer* buffer, char* cstr, intmax_t length);
void buffer_push_cstring_front(struct buffer* buffer, char* cstr, intmax_t length);
void buffer_push_cstring_repeat(struct buffer* buffer, char* value, intmax_t length, intmax_t rept);

char buffer_pop_front(struct buffer* buffer);
void buffer_pop(struct buffer* buffer);

void buffer_delete_at(struct buffer* buffer, intmax_t pos);
void buffer_delete_n_at(struct buffer* buffer, intmax_t n, intmax_t pos);
void buffer_delete_after(struct buffer* buffer, intmax_t pos);
void buffer_delete_before(struct buffer* buffer, intmax_t pos);

struct buffer
{
	unsigned char* data;
	intmax_t length;
	intmax_t capacity;
};

#endif