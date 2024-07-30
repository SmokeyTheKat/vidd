#ifndef __VIDD_PIXEL_HPP__
#define __VIDD_PIXEL_HPP__

#include <vidd/color.hpp>
#include <vidd/charsets.hpp>
#include <vidd/style.hpp>
#include <vidd/wchar.hpp>

#include <string>
#include <cstdint>

struct Pixel {
	Style style;
	WChar character{' '};

	Pixel(void) = default;
	Pixel(WChar character)
	: character(character) {};
	Pixel(WChar character, Style style)
	: style(style), character(character) {};
	Pixel(Style style)
	: style(style), character(' ') {};
	bool operator==(const Pixel& other) const {
		return character == other.character && style == other.style;
	};
};

#endif
