#include <vidd/themes.h>

#include <vidd/syntax.h>
#include <vidd/style.h>

#include <vidd/themes/bumblebee>
#include <vidd/themes/amp>
#include <vidd/themes/monokai>
#include <vidd/themes/future>
#include <vidd/themes/vscode>
#include <vidd/themes/ctx>
#include <vidd/themes/gruvbox>
#include <vidd/themes/gloom>
#include <vidd/themes/strg>

struct theme* active_theme = gloom;

struct theme* themes[] = {
	amp,
	bumblebee,
	monokai,
	future,
	vscode,
	ctx,
	gruvbox,
	gloom,
	strg,
};
int themes_length = sizeof(themes) / sizeof(themes[0]);


void buffer_push_default_style(struct buffer* buffer)
{
	buffer_print(buffer, NOSTYLE);
	buffer_print(buffer, active_theme->bg_style);
	buffer_print(buffer, active_theme->fg_style);
}