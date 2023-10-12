#ifndef __VIDD_CSTRING_HPP__
#define __VIDD_CSTRING_HPP__

#include <cctype>

namespace CString {

constexpr std::size_t nLength(const char* s, std::size_t n) {
   std::size_t i = 0;
   while (i < n && s[i]) i++;
   return i;
}

constexpr std::size_t length(const char* s) {
   std::size_t i = 0;
   while (s[i]) i++;
   return i;
}

}; // namespace CString

#endif
