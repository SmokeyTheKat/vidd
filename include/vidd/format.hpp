#ifndef __VIDD_FORMAT_HPP__
#define __VIDD_FORMAT_HPP__

#ifdef USE_LIBFMT

#include <fmt/format.h>

namespace Format {

using fmt::format;
template<class... Args>
std::string vformat(std::string_view fmt, Args&&... args) {
	return fmt::vformat(fmt, fmt::make_format_args(args...));
}

}; // namespace Format

#else

#include <format>

namespace Format {

using std::format;
template<class... Args>
std::string vformat(std::string_view fmt, Args&&... args) {
	return std::vformat(fmt, std::make_format_args(args...));
}

}; // namespace Format

#endif

#endif
