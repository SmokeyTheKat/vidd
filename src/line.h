#ifndef __vidd_line_h__
#define __vidd_line_h__

#include <ddcMem.h>

struct line
{
    struct line* next;
    struct line* prev;
    char* text;
    long len, cap, num;
};

struct line* make_line(struct line* next, struct line* prev, long num, long cap)
{
    struct line* output = malloc(sizeof(struct line));
    output->next = next;
    output->prev = prev;
    output->cap = cap;
    output->len = 0;
    output->num = num;
    output->text = malloc(cap);
    return output;
}

struct line* line_get_line(struct line* l, long num)
{
    if (l->num == num) return l;
    else if (num < l->num)
        while (l->num != num) l = l->prev;
    else
        while (l->num != num) l = l->next;
    return l;
}
struct line* line_get_first(struct line* l)
{
    struct line* first = l;
    while (l->prev != (0)) l = l->prev;
    return l;
}
struct line* line_get_last(struct line* l)
{
    struct line* first = l;
    while (l->next != (0)) l = l->next;
    return l;
}

void line_redraw_after(struct line* l)
{
    cursor_save();
    while (l != (0))
    {
        cursor_erase_line();
        cursor_return();
        ddPrint(l->text, l->len);
        ddPrint_nl();
        l = l->next;
    }
    cursor_restore();
}
void line_redraw(struct line* l)
{
    cursor_save();
    cursor_erase_line();
    cursor_return();
    ddPrint(l->text, l->len);
    cursor_restore();
}

void line_resize(struct line* l, long size)
{
    if (l->cap < size)
    {
        l->cap = size;
        l->text = realloc(l->text, size+1);
    }
    l->len = size;
}
void line_adjust_num_after(struct line* l, int dx)
{
    while (l)
    {
        l->num += dx;
        l = l->next;
    }
}
void line_split(struct line* l, int x)
{
    struct line* newl = make_line(l->next, l, l->num+1, l->cap);
    l->next = newl;
    if (newl->next) newl->next->prev = newl;
    mem_copy(newl->text, l->text+x, l->len-x);
    newl->len = l->len-x;
    l->len = x;
    line_adjust_num_after(newl->next, 1);
}
void line_append(struct line* l, char c)
{
    if (c == '	')
    {
        line_append(l, ' ');
        line_append(l, ' ');
        line_append(l, ' ');
        line_append(l, ' ');
        return;
    }
    line_resize(l, l->len+1);
    l->text[l->len-1] = c;
}
void line_replace(struct line* l, int x, char c)
{
    l->text[x] = c;
}
void line_insert(struct line* l, int x, char c)
{
    if (c == '	')
    {
        line_insert(l, x, ' ');
        line_insert(l, x, ' ');
        line_insert(l, x, ' ');
        line_insert(l, x, ' ');
        return;
    }
    line_resize(l, l->len+1);
    mem_copy_step(l->text+l->len, l->text+l->len-1, -1, l->len-x);
    l->text[x] = c;
}
void line_delete(struct line* l, int x)
{
    mem_copy(l->text+x, l->text+x+1, l->len-x);
    l->len--;
}

#endif
