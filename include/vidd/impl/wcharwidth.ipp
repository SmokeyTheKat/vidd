#ifndef __VIDD_WCHAR_WIDTH_IPP__
#define __VIDD_WCHAR_WIDTH_IPP__

#include <wchar.h>

namespace WCharWidth {

constexpr std::size_t width(WChar c) {
	WChar utf32;
	if (c.length() == 1) {
		utf32.value = (c.bytes[0] & 0b01111111);
	} else if (c.length() == 2) {
		utf32.value = (c.bytes[0] & 0b00011111) << 6 | (c.bytes[0] & 0b00111111);
	} else if (c.length() == 3) {
		utf32.value = (c.bytes[0] & 0b00001111) << 12 | (c.bytes[1] & 0b00111111) << 6 | (c.bytes[2] & 0b00111111);
	} else {
		utf32.value = (c.bytes[0] & 0b00000111) << 18 | (c.bytes[1] & 0b00111111) << 12 | (c.bytes[2] & 0b00111111) << 6 | (c.bytes[3] & 0b00111111);
	}

	return ::wcwidth(utf32.value);
}

}; // namespace WCharWidth

#endif
