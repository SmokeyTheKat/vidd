#ifndef __VIDD_STYLE_HPP__
#define __VIDD_STYLE_HPP__

#include <vidd/color.hpp>

#include <string>

#define CURSOR_HIDE "\e[?25l"
#define CURSOR_SHOW "\e[?25h"

#define FRGB(r, g, b) "\e[38;2;" r ";" g ";" b "m"
#define BRGB(r, g, b) "\e[48;2;" r ";" g ";" b "m"
#define FGSC(s) "\e[38;2;" s ";" s ";" s  "m"
#define BGSC(s) "\e[48;2;" s ";" s ";" s  "m"

#define NOSTYLE "\e[0m"
#define STYLE_UNDERLINE "\e[4m"
#define STYLE_REVERSE "\e[7m"
#define STYLE_ITALIC "\e[3m"
#define STYLE_BOLD "\e[1m"

#define STYLE_ID_UNDERLINE "4"
#define STYLE_ID_REVERSE "7"
#define STYLE_ID_ITALIC "3"
#define STYLE_ID_BOLD "1"

#define STYLE_ID_NOT_UNDERLINE "24"
#define STYLE_ID_NOT_REVERSE "27"
#define STYLE_ID_NOT_ITALIC "23"
#define STYLE_ID_NOT_BOLD "22"

#define FG_BLACK "\e[38;2;0;0;0m"
#define FG_WHITE "\e[38;2;255;255;255m"
#define FG_RED "\e[38;2;255;0;0m"
#define FG_GREEN "\e[38;2;0;255;0m"
#define FG_LIGHT_GREEN "\e[38;2;144;238;144m"
#define FG_BLUE "\e[38;2;0;0;255m"
#define FG_LIGHT_BLUE "\e[38;2;173;216;230m"
#define FG_YELLOW "\e[38;2;255;255;0m"
#define FG_MAGENTA "\e[38;2;255;0;255m"
#define FG_CYAN "\e[38;2;0;255;255m"
#define FG_LIGHT_CYAN "\e[38;2;55;255;255m"
#define FG_DARK_CYAN "\e[38;2;0;160;160m"

#define FG_ORANGE "\e[38;2;255;165;0m"
#define FG_DARK_ORANGE "\e[38;2;200;110;00m"
#define FG_LIGHT_ORANGE "\e[38;2;255;175;20m"

#define BG_BLACK "\e[48;2;0;0;0m"
#define BG_WHITE "\e[48;2;255;255;255m"
#define BG_RED "\e[48;2;255;0;0m"
#define BG_GREEN "\e[48;2;0;255;0m"
#define BG_LIGHT_GREEN "\e[48;2;144;238;144m"
#define BG_BLUE "\e[48;2;0;0;255m"
#define BG_LIGHT_BLUE "\e[48;2;173;216;230m"
#define BG_YELLOW "\e[48;2;255;255;0m"
#define BG_MAGENTA "\e[48;2;255;0;255m"
#define BG_CYAN "\e[48;2;0;255;255m"
#define BG_ORANGE "\e[48;2;255;165;0m"
#define BG_LIGHT_ORANGE "\e[48;2;255;175;20m"

using StyleFlags = uint8_t;

struct Style {
	StyleFlags format = 0;
	Color fg;
	Color bg;

	enum {
		none =      (0),
		underline = (1 << 1),
		reverse =   (1 << 2),
		bold =      (1 << 3),
		italic =    (1 << 4),
	};

	Style(void) = default;
	Style(StyleFlags format)
	: format(format) {};
	Style(Color fg, Color bg)
	: format(0), fg(fg), bg(bg) {};
	Style(Color fg, Color bg, StyleFlags format)
	: format(format), fg(fg), bg(bg) {};

	std::string string(void) const {
		std::string out;
		string(out);
		return out;
	};

	void string(std::string& out) const {
		out += NOSTYLE;
		if (format != 0) {
			out += "\e[";
			if (format & Style::underline) out += STYLE_ID_UNDERLINE ";";
			if (format & Style::reverse) out += STYLE_ID_REVERSE ";";
			if (format & Style::bold) out += STYLE_ID_BOLD ";";
			if (format & Style::italic) out += STYLE_ID_ITALIC ";";
			out.pop_back();
			out += "m";
		}
		out += fg.fgStr();
		out += bg.bgStr();
	};

	void differenceString(std::string& out, const Style& other) const {
		StyleFlags diff = format ^ other.format;
		if (diff) {
			out += "\e[";
			if (diff & Style::underline) {
				out += (other.format & Style::underline) ? STYLE_ID_UNDERLINE ";" : STYLE_ID_NOT_UNDERLINE ";";
			}
			if (diff & Style::reverse) {
				out += (other.format & Style::reverse) ? STYLE_ID_REVERSE ";" : STYLE_ID_NOT_REVERSE ";";
			}
			if (diff & Style::bold) {
				out += (other.format & Style::bold) ? STYLE_ID_BOLD ";" : STYLE_ID_NOT_BOLD ";";
			}
			if (diff & Style::italic) {
				out += (other.format & Style::italic) ? STYLE_ID_ITALIC ";" : STYLE_ID_NOT_ITALIC ";";
			}
			out.pop_back();
			out += "m";
		}

		if (fg != other.fg) out += other.fg.fgStr();
		if (bg != other.bg) out += other.bg.bgStr();
	};

	friend Style operator+(const Style& lhs, const Style& rhs) {
		Style out = lhs;
		if (rhs.fg) out.fg = rhs.fg;
		if (rhs.bg) out.bg = rhs.bg;
		out.format |= rhs.format;
		return out;
	};

	void operator+=(const Style& other) {
		if (other.fg) fg = other.fg;
		if (other.bg) bg = other.bg;
		format |= other.format;
	};

	bool operator==(const Style& other) const {
		return format == other.format && fg == other.fg && bg == other.bg;
	};

	bool operator!=(const Style& other) const {
		return !(*this == other);
	};
};

#endif
