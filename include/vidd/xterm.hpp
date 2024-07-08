#ifndef __VIDD_XTERM_HPP__
#define __VIDD_XTERM_HPP__

#define MAKE_MASK(_name, _mask, _offset) constexpr auto _name(auto v) { return (v & _mask) >> _offset; }

namespace XTerm {
	MAKE_MASK(maskMouseEventCtrl, 0b00010000, 4);
	MAKE_MASK(maskMouseEventAlt, 0b00010000, 3);

	MAKE_MASK(maskMouseEventType, 0b11100000, 5);
	enum MouseEventType {
		Move = 0b010,
		Click = 0b001,
		Scroll = 0b011,
	};

	MAKE_MASK(maskMouseEventButton, 0b00000011, 0);
	enum MouseEventButton {
		Left = 0b00,
		Middle = 0b01,
		Right = 0b10,
		Release = 0b11,
		None = 0b11,
	};

	MAKE_MASK(maskMouseEventScroll, 0b00000001, 0);
	enum MouseEventScroll {
		Up = 0b0,
		Down = 0b1,
	};
};

#endif
