#ifndef __VIDD_PIXEL_HPP__
#define __VIDD_PIXEL_HPP__

#include <vidd/color.hpp>
#include <vidd/charsets.hpp>
#include <vidd/style.hpp>
#include <vidd/wchar.hpp>

#include <string>
#include <cstdint>

struct Pixel {
    WChar character{' '};
    Style style;

    Pixel(void) = default;
    Pixel(WChar character)
    : character(character) {};
    Pixel(WChar character, Style style)
    : character(character), style(style) {};
    Pixel(Style style)
    : character(' '), style(style) {};
    bool operator==(const Pixel& other) const {
        return character == other.character && style == other.style;
    };
};

#endif
