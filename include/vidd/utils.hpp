#ifndef __VIDD_UTILS_HPP__
#define __VIDD_UTILS_HPP__

#include <vidd/charsets.hpp>

#include <string>
#include <vector>
#include <cstdint>
#include <string_view>
#include <ctime>
#include <cmath>

namespace Utils {

template<class T>
inline T roundTo(T v, T to) {
	return to * std::round(v / to);
}

inline int countDigits(int n) {
	if (n < 10) return 1;
	if (n < 100) return 2;
	if (n < 1000) return 3;
	if (n < 10000) return 4;
	if (n < 100000) return 5;
	if (n < 1000000) return 6;
	if (n < 10000000) return 7;
	if (n < 100000000) return 8;
	return 9;
}

inline std::string stringToLower(std::string str) {
	for (auto& c : str) {
		c = std::tolower(c);
	}
	return str;
}

inline std::string joinStrings(const std::vector<std::string>& strings, std::string_view delim) {
	if (strings.size() == 0) return "";
	std::string out(strings[0]);
	for (std::size_t i = 1; i < strings.size(); i++) {
		out += delim;
		out += strings[i];
	}
	return out;
}

inline std::string getTimeString(void) {
	std::time_t time = std::time(nullptr);
	char timeBuf[std::size("hh:mm:ss")];
	std::strftime(timeBuf, std::size(timeBuf), "%T", std::localtime(&time));
	return std::string(timeBuf);
}

constexpr char toLower(char c) {
	if (c >= 'A' && c <= 'Z') return c + 32;
	return c;
}

constexpr char toUpper(char c) {
	if (c >= 'a' && c <= 'z') return c - 32;
	return c;
}

constexpr char isSpace(char c) {
	switch (c) {
	case ' ': return true;
	case '\f': return true;
	case '\n': return true;
	case '\r': return true;
	case '\t': return true;
	case '\v': return true;
	default: return false;
	}
}

constexpr int stringToInt(std::string_view str, int base) {
	std::string_view baseChars[32];
	baseChars[10] = "0123456789";
	baseChars[16] = "0123456789abcdef";
	std::string_view validBaseChars[32];
	validBaseChars[10] = "0123456789";
	validBaseChars[16] = "0123456789abcdefABCDEF";

	std::size_t start = str.find_first_of(validBaseChars[base]);
	if (start == std::string_view::npos) {
		return 0;
	}

	int sign = (start > 0 && str[start - 1] == '-') ? -1 : 1;
	str.remove_prefix(start);

	std::size_t end = str.find_first_not_of(validBaseChars[base]);
	if (end != std::string_view::npos) {
		str.remove_suffix(str.length() - end);
	}

	int value = 0;
	int multiplier = 1;
	for (std::ptrdiff_t i = str.length() - 1; i >= 0; i--) {
		int num = baseChars[base].find_first_of(toLower(str[i]));
		value += num * multiplier * sign;
		multiplier *= base;
	}

	return value;
}

void stringReplace(std::string& string, std::string_view find, std::string_view replace);
std::string repeatString(std::string_view str, int n);
bool stringIsUppercase(std::string_view str);
std::string getPathBaseName(std::string path);

}; // namepsace Utils

#endif
