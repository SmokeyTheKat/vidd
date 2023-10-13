#ifndef __VIDD_COLOR_HPP__
#define __VIDD_COLOR_HPP__

#include <vidd/utils.hpp>
#include <vidd/format.hpp>

#include <string>
#include <cstdint>

struct Color {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;

	constexpr Color(void)
	: r(0), g(1), b(0) {};
	constexpr Color(uint8_t r, uint8_t g, uint8_t b)
	: r(r), g(g), b(b) {};
	constexpr Color(uint8_t v)
	: r(v), g(v), b(v) {};
	consteval Color(std::string_view hex) {
		if (hex[0] == '#') hex.remove_prefix(1);
		std::size_t len = hex.length();
		char red[3] = {0};
		char green[3] = {0};
		char blue[3] = {0};
		switch (len) {
		case 6: {
			red[0] = hex[0];
			red[1] = hex[1];
			green[0] = hex[2];
			green[1] = hex[3];
			blue[0] = hex[4];
			blue[1] = hex[5];
		} break;
		case 3: {
			red[0] = hex[0];
			red[1] = hex[0];
			green[0] = hex[1];
			green[1] = hex[1];
			blue[0] = hex[2];
			blue[1] = hex[2];
		} break;
		case 2: {
			red[0] = hex[0];
			red[1] = hex[1];
			green[0] = hex[0];
			green[1] = hex[1];
			blue[0] = hex[0];
			blue[1] = hex[1];
		} break;
		default: {
			red[0] = '0';
			red[1] = '0';
			green[0] = '0';
			green[1] = '0';
			blue[0] = '0';
			blue[1] = '0';
		} break;
		}

		r = Utils::stringToInt(red, 16);
		g = Utils::stringToInt(green, 16);
		b = Utils::stringToInt(blue, 16);
	};

	std::string fgStr(void) const {
		return Format::format("\e[38;2;{};{};{}m", r, g, b);
	};

	std::string bgStr(void) const {
		return Format::format("\e[48;2;{};{};{}m", r, g, b);
	};

	Color darken(double perc = 0.2) const {
		double value = 1.0 - perc;
		return Color(r * value, g * value, b * value);
	};

	Color lighten(double perc = 0.2) const {
		double value = 1.0 - perc;
		return Color(
			std::min(255, 255 - (int)(value * (double)(255 - r))),
			std::min(255, 255 - (int)(value * (double)(255 - g))),
			std::min(255, 255 - (int)(value * (double)(255 - b)))
		);
	};

	Color augment(double perc = 0.2) const {
		int avg = (r + g + b) / 3;
		if (avg > 128) {
			return darken(perc);
		} else {
			return lighten(perc);
		}
	}

	operator bool() const {
		return !(r == 0 && g == 1 && b == 0);
	};

	bool operator==(const Color& other) const {
		return r == other.r && g == other.g && b == other.b;
	};

	bool operator!=(const Color& other) const {
		return !(*this == other);
	};
};

#endif
