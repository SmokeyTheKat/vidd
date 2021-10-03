#ifndef __VIDD_BUFFER_H__
#define __VIDD_BUFFER_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define BUFFER_SPACER 20

struct buffer;

struct buffer make_buffer(intmax_t capacity);
void free_buffer(struct buffer* buffer);

void buffer_copy(struct buffer* dest, struct buffer* src);
void buffer_copy_inverse(struct buffer* dest, struct buffer* src);

void buffer_printf(struct buffer* buffer, const char* fmt, ...);
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

struct buffer make_buffer_from_cstring(char* cstr)
{
	intmax_t length = strlen(cstr);
	struct buffer output = make_buffer(length + 10);
	buffer_set_data(&output, cstr, length);
	return output;
}
struct buffer make_buffer(intmax_t capacity)
{
	struct buffer output;
	output.data = calloc(sizeof(char), capacity);
	output.capacity = capacity;
	output.length = 0;
	return output;
}

void free_buffer(struct buffer* buffer)
{
	if (buffer->data) free(buffer->data);
}

void buffer_copy(struct buffer* dest, struct buffer* src)
{
	if (src->length >= dest->capacity)
	{
		dest->capacity = src->length + BUFFER_SPACER;
		dest->data = realloc(dest->data, dest->capacity);
	}
	dest->length = src->length;
	memcpy(dest->data, src->data, src->length);
	dest->data[dest->length] = 0;
}

void buffer_copy_inverse(struct buffer* dest, struct buffer* src)
{
	if (src->length >= dest->capacity)
	{
		dest->capacity = src->length + BUFFER_SPACER;
		dest->data = realloc(dest->data, dest->capacity);
	}
	dest->length = src->length;
	for (intmax_t i = 0, j = src->length - 1; i < src->length && j >= 0; i++, j--)
	{
		dest->data[i] = src->data[j];
	}
	dest->data[dest->length] = 0;
}

void buffer_print(struct buffer* buffer, char* data)
{
	intmax_t length = strlen(data);
	buffer_push_cstring(buffer, data, length);
}
void buffer_printf(struct buffer* buffer, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	va_list cpyargs;
	va_copy(cpyargs, args);

	intmax_t fmtbuf_length = vsnprintf(0, 0, fmt, args);
	char* fmtbuf = malloc(fmtbuf_length + 1);
	vsnprintf(fmtbuf, fmtbuf_length + 1, fmt, cpyargs);

	buffer_push_cstring(buffer, fmtbuf, fmtbuf_length);

	free(fmtbuf);

	va_end(args);
	va_end(cpyargs);
}

void buffer_insert_at(struct buffer* buffer, char value, intmax_t pos)
{
	if (buffer->length + 1 >= buffer->capacity)
	{
		buffer->capacity = buffer->capacity + BUFFER_SPACER;
		buffer->data = realloc(buffer->data, buffer->capacity);
	}
	for (intmax_t i = buffer->length-1; i >= pos; i--)
	{
		buffer->data[i+1] = buffer->data[i];
	}
	buffer->data[pos] = value;
	buffer->length++;
	buffer->data[buffer->length] = 0;
}

void buffer_set_data(struct buffer* buffer, char* data, intmax_t length)
{
	if (length >= buffer->capacity)
	{
		buffer->capacity = length + BUFFER_SPACER;
		buffer->data = realloc(buffer->data, buffer->capacity);
	}
	memcpy(buffer->data, data, length);
	buffer->length = length;
	buffer->data[buffer->length] = 0;
}

void buffer_clear(struct buffer* buffer)
{
	buffer->length = 0;
	memset(buffer->data, 0, buffer->capacity);
}

void buffer_push(struct buffer* buffer, char value)
{
	if (buffer->length + 1 >= buffer->capacity)
	{
		buffer->capacity = buffer->capacity + BUFFER_SPACER;
		buffer->data = realloc(buffer->data, buffer->capacity);
	}
	buffer->data[buffer->length++] = value;
	buffer->data[buffer->length] = 0;
}

void buffer_push_repeat(struct buffer* buffer, char value, intmax_t rept)
{
	for (intmax_t i = 0; i < rept; i++) buffer_push(buffer, value);
}

void buffer_push_cstring_front(struct buffer* buffer, char* cstr, intmax_t length)
{
	if (buffer->length + length >= buffer->capacity)
	{
		buffer->capacity += length + BUFFER_SPACER;
		buffer->data = realloc(buffer->data, buffer->capacity);
	}
	for (intmax_t i = buffer->length; i >= 0; i--)
		buffer->data[i + length] = buffer->data[i];
	for (intmax_t i = 0; i < length; i++)
		buffer->data[i] = cstr[i];
	buffer->length += length;
	
}
void buffer_push_cstring(struct buffer* buffer, char* cstr, intmax_t length)
{
	if (buffer->length + length >= buffer->capacity)
	{
		buffer->capacity += length + BUFFER_SPACER;
		buffer->data = realloc(buffer->data, buffer->capacity);
	}
	memcpy(&buffer->data[buffer->length], cstr, length);
	buffer->length += length;
	buffer->data[buffer->length] = 0;
}

void buffer_push_cstring_repeat(struct buffer* buffer, char* value, intmax_t length, intmax_t rept)
{
	for (intmax_t i = 0; i < rept; i++) buffer_push_cstring(buffer, value, length);
}

char buffer_pop_front(struct buffer* buffer)
{
	char value = buffer->data[0];
	for (int i = 0; i < buffer->length-1; i++)
		buffer->data[i] = buffer->data[i+1];
	buffer->length--;
	buffer->data[buffer->length] = 0;
	return value;
}

void buffer_pop(struct buffer* buffer)
{
	buffer->data[--buffer->length] = 0;
}

void buffer_delete_at(struct buffer* buffer, intmax_t pos)
{
	if (buffer->length == 0) return;
	for (intmax_t i = pos; i < buffer->length - 1; i++)
	{
		buffer->data[i] = buffer->data[i+1];
	}
	buffer->length--;
	buffer->data[buffer->length] = 0;
}

void buffer_delete_n_at(struct buffer* buffer, intmax_t n, intmax_t pos)
{
	for (intmax_t i = 0; i < n; i++)
		buffer_delete_at(buffer, pos);
}

void buffer_delete_after(struct buffer* buffer, intmax_t pos)
{
	if (buffer->length == 0) return;
	buffer->length -= buffer->length - pos;
	buffer->data[buffer->length] = 0;
}

void buffer_delete_before(struct buffer* buffer, intmax_t pos)
{
	if (buffer->length == 0) return;
	for (intmax_t i = 0, j = pos; j < buffer->length; i++, j++)
	{
		buffer->data[i] = buffer->data[j];
	}
	buffer->length -= pos;
	buffer->data[buffer->length] = 0;
}

#endif
