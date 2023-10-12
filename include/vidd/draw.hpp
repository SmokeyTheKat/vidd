#ifndef __VIDD_DRAW_HPP__
#define __VIDD_DRAW_HPP__

#include <vidd/framebuffer.hpp>
#include <vidd/pixel.hpp>
#include <vidd/pos.hpp>
#include <vidd/terminal.hpp>
#include <vidd/style.hpp>
#include <vidd/wchar.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace Draw {
	struct Line {
		WChar vLine;
		WChar hLine;
		WChar tlCorner;
		WChar trCorner;
		WChar blCorner;
		WChar brCorner;
	};

	const Line normalLine = {
//        "│",
//        "─",
//        "┌",
//        "┐",
//        "└",
//        "┘",
		"│",
		"─",
		"╭",
		"╮",
		"╰",
		"╯",
	};

	const Line thickLine = {
		"┃",
		"━",
		"┏",
		"┓",
		"┗",
		"┛",
	};

	const Line doubleLine = {
		"║",
		"═",
		"╔",
		"╗",
		"╚",
		"╝",
	};

	const Line dottedLine = {
		"┆",
		"┄",
		" ",
		" ",
		" ",
		" ",
	};

	const Line dashedLine = {
		"|",
		"-",
		"/",
		"\\",
		"\\",
		"/",
	};

	const Line squareLine = {
		"|",
		"-",
		"+",
		"+",
		"+",
		"+",
	};

	void style(Style style);

	void box(Vec2 tl, Vec2 size, WChar chr, FrameBuffer* fb = Terminal::getFrameBuffer());
	void box(Vec2 tl, Vec2 size, Draw::Line line, FrameBuffer* fb = Terminal::getFrameBuffer());

	void filledBox(Vec2 tl, Vec2 size, WChar chr, FrameBuffer* fb = Terminal::getFrameBuffer());

	void paintFormat(Vec2 tl, Vec2 size, Style format, FrameBuffer* fb = Terminal::getFrameBuffer());

	void vLine(Vec2 pos, int length, WChar chr, FrameBuffer* fb = Terminal::getFrameBuffer());
	void vLine(Vec2 pos, int length, Draw::Line line, FrameBuffer* fb = Terminal::getFrameBuffer());

	void hLine(Vec2 pos, int length, WChar chr, FrameBuffer* fb = Terminal::getFrameBuffer());
	void hLine(Vec2 pos, int length, Draw::Line line, FrameBuffer* fb = Terminal::getFrameBuffer());

	void text(Vec2 pos, WStringView text, FrameBuffer* fb = Terminal::getFrameBuffer());
};

#endif
