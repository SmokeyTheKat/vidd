#ifndef __VIDD_THEME_HPP__
#define __VIDD_THEME_HPP__

#include <vidd/style.hpp>
#include <vidd/syntax.hpp>

#include <string_view>
#include <array>
#include <vector>

struct Theme {
	std::string_view name;
	Style fg;
	Style bg;
	Style text;
	Color important;
	Style highlight;
	Style topBarActive;
	Style topBarInactive;
	std::string_view modeFormat;
	Style barActive;
	Style barInactive;
	std::string_view numberFormat;
	Style numberStyle;
	std::string_view emptyLineFormat;
	Style emptyLineStyle;
	std::array<Style, static_cast<int>(StyleType::StyleTypeLength)> syntaxStyles;

	constexpr const Style& getSyntaxStyle(StyleType type) const {
		return syntaxStyles[static_cast<int>(type)];
	};
};

namespace Themes {

const Theme* getThemeByName(std::string_view name);
std::vector<const Theme*> getAllThemes(void);

};

#endif
