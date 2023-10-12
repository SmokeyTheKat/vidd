#ifndef __VIDD_PARSE_STRING_HPP__
#define __VIDD_PARSE_STRING_HPP__

#include <vidd/cstring.hpp>
#include <vidd/utils.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <cstring>

class ParseString {
	const char* mOrigData;
	std::size_t mOrigLength;
	const char* mData;
	std::size_t mLength;

public:
	static constexpr std::size_t npos = std::size_t(-1);

	constexpr ParseString(const char* data);
	constexpr ParseString(const char* data, std::size_t length);
	constexpr ParseString(std::string_view str);
	constexpr ParseString(const std::string& str);
	constexpr ParseString(const ParseString& other);

	constexpr std::string string(void) const;
	constexpr std::string_view view(void) const;

	constexpr long getInt(int base = 10) const;

	constexpr std::size_t length(void) const;
	constexpr bool isEmpty(void) const;

	constexpr char front(void) const;
	constexpr char back(void) const;

	constexpr ParseString subString(std::size_t begin, std::size_t end = ParseString::npos) const;

	constexpr ParseString pop(std::size_t count = 1);
	constexpr ParseString popBack(std::size_t count = 1);
	constexpr ParseString popWhile(auto&& func);
	constexpr ParseString popBackWhile(auto&& func);
	constexpr ParseString popUntil(char until);
	constexpr ParseString popUntil(ParseString until);
	constexpr ParseString popUntilAndSkip(char until);
	constexpr ParseString popUntilAndSkip(ParseString until);

	constexpr std::vector<ParseString> split(char at) const;
	constexpr std::vector<ParseString> split(ParseString at) const;

	constexpr ParseString strip(void) const;

	constexpr std::size_t find(char chr) const;
	constexpr std::size_t find(ParseString str) const;

	constexpr std::size_t parsedCount(void) const;

	constexpr char operator[](int idx) const;
	constexpr char operator*(void) const;

	constexpr bool operator==(const char* other) const;
	constexpr bool operator==(const ParseString& other) const;
	constexpr bool operator==(std::string_view other) const;

private:
	constexpr std::size_t indexElse(std::size_t val) const;
};

#include "impl/parsestring.ipp"

#endif
