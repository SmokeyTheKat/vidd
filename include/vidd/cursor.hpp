#ifndef __VIDD_CURSOR_HPP__
#define __VIDD_CURSOR_HPP__

#include <vidd/line.hpp>
#include <vidd/pos.hpp>

struct Cursor {
	int x = 0;
	int lx = 0;
	Line* y = nullptr;

	Cursor(void)
	: x(0), y(nullptr) {};

	Cursor(int x, Line* line)
	: x(x), y(line) {};

	Cursor(int x, Line* line, int lx)
	: x(x), lx(x), y(line) {};

	Vec2 toVec2(void) {
		if (y == nullptr) return Vec2::zero();
		return Vec2(x, y->number);
	};
};

#endif