#include <vidd/theme.hpp>

#include <vidd/color.hpp>

namespace {

#include <vidd/themes/gloom.hpp>
#include <vidd/themes/bumblebee.hpp>
#include <vidd/themes/strg.hpp>
#include <vidd/themes/notepad.hpp>
#include <vidd/themes/nyquil.hpp>
#include <vidd/themes/hacker.hpp>
#include <vidd/themes/dracula.hpp>
const std::vector<const Theme*> themes {
	&gloom,
	&bumblebee,
	&strg,
	&notepad,
	&nyquil,
	&hacker,
	&dracula,
};

}; // namespace

namespace Themes {

const Theme* getThemeByName(std::string_view name) {
	for (const auto* theme : themes) {
		if (theme->name == name) return theme;
	}
	return &dracula;
}

std::vector<const Theme*> getAllThemes(void) {
	return themes;
}

}; // namespace Themes
