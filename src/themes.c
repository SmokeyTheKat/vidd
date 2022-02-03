#include <vidd/themes.h>

#include <vidd/syntax.h>
#include <vidd/style.h>

#include <vidd/themes/bumblebee>
#include <vidd/themes/amp>
#include <vidd/themes/monokai>
#include <vidd/themes/future>
#include <vidd/themes/vscode>

struct theme* active_theme = future;

struct theme* themes[] = {
	amp,
	bumblebee,
	monokai,
	future,
	vscode,
};
int themes_length = sizeof(themes) / sizeof(themes[0]);
