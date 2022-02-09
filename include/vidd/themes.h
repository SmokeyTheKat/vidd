#ifndef __VIDD_THEMES_H__
#define __VIDD_THEMES_H__

#include <vidd/buffer.h>

struct theme;

void buffer_push_default_style(struct buffer* buffer);

struct theme
{
	char* name;
	char** syntax_styles;
	char* mode_format;
	char* fg_style;
	char* bg_style;
	char* highlight_style;
	char* line_number_format;
	char* line_number_spacer_style;
	char* empty_line_format;
	char* status_bar_active_style;
	char* status_bar_inactive_style;
};

extern struct theme* active_theme;

extern struct theme* themes[];
extern int themes_length;

#endif