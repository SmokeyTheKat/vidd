#ifndef __VIDD_KEYS_HPP__
#define __VIDD_KEYS_HPP__

#include <cstdint>

typedef uint32_t Key;

namespace Keys {
	constexpr char ctrl(char c) { return c - 96; }
	enum {
		Backspace=127,
		CtrlBackslash=28,
		Return=13,
		ShiftReturn=438,
		Escape=27,
		CtrlBackspace=8,
		CtrlSpace=0,
		Tab='\t',
		Up=1000,
		Down,
		Right,
		Left,
		CtrlUp,
		CtrlDown,
		CtrlRight,
		CtrlLeft,
		Delete,
		CtrlDelete,
		Home,
		End,
		PageUp,
		PageDown,
		ScrollDown,
		ScrollUp,
		ScrollLeft,
		ScrollRight,
		MiddleMouseDown,
		ShiftTab,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
	};
};

#endif
