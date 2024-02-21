#ifndef __VIDD_WCHAR_WIDTH_HPP__
#define __VIDD_WCHAR_WIDTH_HPP__

#include <vidd/wchar.hpp>

#include <cstdint>

namespace WCharWidth {

constexpr std::size_t width(WChar c, bool is_cjk);

}; // namespace WCharWidth

#include "impl/wcharwidth.ipp"

#endif
