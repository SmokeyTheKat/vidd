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
};
int themes_length = sizeof(themes) / sizeof(themes[0]);
