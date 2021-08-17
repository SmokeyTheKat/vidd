#ifndef __VIDD_FRAME_H__
#define __VIDD_FRAME_H__

#include "./utils.h"
#include "./buffer.h"

struct frame
{
	intmax_t x, y;
	intmax_t width, height;
	struct buffer buffer;
	intmax_t cx, cy;
};

void frame_draw_border(struct frame* frame)
{
	cursor_move_to(frame->x, frame->y);
	printf("┌");
	for (intmax_t i = 0; i < frame->width-2; i++)
		printf("─");
	printf("┐");


	cursor_move_to(frame->x, frame->y+1);
	for (intmax_t i = 0; i < frame->height-2; i++)
		printf("│" CURSOR_LEFT() CURSOR_DOWN());
	printf("└");


	cursor_move_to(frame->x + frame->width-1, frame->y+1);
	for (intmax_t i = 0; i < frame->height-2; i++)
		printf("│" CURSOR_LEFT() CURSOR_DOWN());
	printf("┘");

	cursor_move_to(frame->x+1, frame->y + frame->height-1);
	for (intmax_t i = 0; i < frame->width-2; i++)
		printf("─");
}

void frame_clear(struct frame* frame)
{
	for (intmax_t y = frame->y + 1; y < frame->y + frame->height - 1; y++)
	{
		cursor_move_to(frame->x + 1, y);
		for (intmax_t x = 0; x < frame->width - 2; x++)
		{
			printf(" ");
		}
	}
}

void frame_write_text(struct frame* frame)
{
	intmax_t i = 0;
	for (intmax_t y = frame->y + 1; y < frame->y + frame->height - 1; y++)
	{
		cursor_move_to(frame->x + 1, y);
		for (intmax_t x = 0; x < frame->width - 2; x++)
		{
			if (i >= frame->buffer.length) return;
			char c = frame->buffer.data[i];
			if (c == '\n') { i++; break; }
			printf("%c", c);
			i++;
		}
	}
}

void frame_draw(struct frame* frame)
{
	cursor_save();
	frame_draw_border(frame);
	frame_clear(frame);
	frame_write_text(frame);
	cursor_restore();
}

#endif
