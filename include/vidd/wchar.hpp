#ifndef __VIDD_WCHAR_HPP__
#define __VIDD_WCHAR_HPP__

#include <vidd/utils.hpp>
#include <vidd/cstring.hpp>
#include <vidd/parsestring.hpp>

#include <cstdint>
#include <cstring>
#include <vector>
#include <string_view>
#include <map>
#include <limits>
#include <algorithm>
#include <string_view>
#include <stdexcept>

template<class Key, class Value>
class ConstExprMap {
	std::vector<std::pair<Key, Value>> mData;
public:
	constexpr void insert(const Key& key, const Value& value) {
		mData.push_back({key, value});
	};

	constexpr const Value& operator[](const Key& key) const {
		const auto it = std::find_if(
			std::begin(mData),
			std::end(mData),
			[&key](const auto& v) { return v.first == key; }
		);

		if (it != std::end(mData)) {
			return it->second;
		} else {
			throw std::range_error("Not Found");
		}
	};
};

namespace {

//ConstExprMap<std::uint32_t, int> parseEastAsianWidths(ParseString text) {
//    ConstExprMap<std::uint32_t, int> charWidthMap;
//    while (text.isEmpty() == false) {
//        ParseString line = text.popUntilAndSkip('\n');
//        if (line.isEmpty() || line[0] == '#') continue;
//
//        ParseString preComment = line.split('#')[0];
//        std::vector<ParseString> tokens = preComment.split(';');
//        if (tokens.size() < 2) continue;
//
//        ParseString charRange = tokens[0];
//        ParseString width = tokens[1].strip();
//
//        std::vector<ParseString> rangeTokens = charRange.split("..");
//        std::uint32_t charStart = rangeTokens[0].getInt(16);
//        std::uint32_t charEnd = rangeTokens[rangeTokens.size() > 1 ? 1 : 0].getInt(16);
//    
//        for (std::uint32_t c = charStart; c <= charEnd; c++) {
//            if (width == "W" || width == "F") {
//                charWidthMap.insert(c, 2);
//            } else if (width == "Na" || width == "H") {
//                charWidthMap.insert(c, 1);
//            }
//        }
//    }
//    return charWidthMap;
//};

//constexpr ConstExprMap<std::uint32_t, int> eastAsianWidths = parseEastAsianWidths(eastAsianWidthText);
//
}; // anon

struct WChar {
	union {
		char bytes[4];
		std::uint32_t value;
	};

	constexpr static std::size_t utf8len(const char* str) {
		int v0 = (*str & 0x80) >> 7;
		int v1 = (*str & 0x40) >> 6;
		int v2 = (*str & 0x20) >> 5;
		int v3 = (*str & 0x10) >> 4;
		int length = 1 + v0 * v1 + v0 * v1 * v2 + v0 * v1 * v2 * v3;
		return length;
	};

	constexpr static bool utf8valid(const char* str) {
		if (str == nullptr) return false;
		return (
			(str[0] & 0x80) == 0 ||
			((str[0] & 0xE0) == 0xC0 && (str[1] & 0xC0) == 0x80) ||
			((str[0] & 0xF0) == 0xE0 && (str[1] & 0xC0) == 0x80 && (str[2] & 0xC0) == 0x80) ||
			((str[0] & 0xF8) == 0xF0 && (str[1] & 0xC0) == 0x80 && (str[2] & 0xC0) == 0x80 && (str[3] & 0xC0) == 0x80)
		);
	};

	constexpr static WChar spacer(void) { return WChar(1); };

	constexpr WChar(void) = default;

	constexpr WChar(std::uint32_t value)
	: value(value) {};

	constexpr WChar(char chr) {
		value = 0;
		bytes[0] = chr;
	};

	constexpr WChar(int value)
	: value(value) {};

	constexpr WChar(const char* cstr) {
		value = 0;
		if (utf8valid(cstr)) {
			std::size_t len = utf8len(cstr);
			for (std::size_t i = 0; i < len; i++) {
				bytes[i] = cstr[i];
			}
			if (len < 4) {
				bytes[len] = 0;
			}
		}
	};

	constexpr WChar(const char* cstr, std::size_t cstrLen) {
		value = 0;
		std::size_t len = utf8len(cstr);
		if (len <= cstrLen) {
			for (std::size_t i = 0; i < len; i++) {
				bytes[i] = cstr[i];
			}
			if (len < 4) {
				bytes[len] = 0;
			}
		}
	};

	constexpr std::string string(void) const {
		return std::string(bytes, length());
	};

	constexpr std::string_view view(void) const {
		return std::string_view(bytes, length());
	};

	constexpr std::size_t length(void) const {
		if (bytes[0] == 0) return 0;
		if (bytes[1] == 0) return 1;
		if (bytes[2] == 0) return 2;
		if (bytes[3] == 0) return 3;
		return 4;
	};

	constexpr std::size_t visibleLength(void) const;

	constexpr bool operator==(const WChar& other) const {
		return value == other.value;
	};

	constexpr bool operator==(char chr) const {
		return length() == 1 && bytes[0] == chr;
	};

	constexpr bool operator==(uint32_t intValue) const {
		return value == intValue;
	};

	constexpr operator char(void) const {
		return static_cast<char>(bytes[0]);
	};
};

static inline bool isVisibleWChar(WChar chr) {
	return chr.view().length() > 1 || CharSets::nonescape.contains(chr);
};

class WStringView;

class WString {
	std::vector<WChar> mData;

public:
	using iterator = std::vector<WChar>::iterator;
	using const_iterator = std::vector<WChar>::const_iterator;

	constexpr WString(void);
	template<std::convertible_to<std::string_view> T>
	constexpr WString(T&& svLike);
	constexpr WString(WStringView view);
	constexpr WString(std::size_t n, WChar chr);
	constexpr WString(const WString& other);
	constexpr WString(WString&& other);

	WString& operator=(const WString& other);
	WString& operator=(WString&& other);

	constexpr const WChar* data(void) const;
	constexpr std::size_t length(void) const;
	constexpr std::string string(void) const;

	void pushBack(const WChar& chr);
	void popBack(void);

	template <class T>
	void append(const T& str);

	template <class IT>
	void insert(iterator at, IT from, IT to);
	void insert(std::size_t at, WChar chr);
	void insert(std::size_t at, WChar chr, std::size_t n);

	void remove(iterator begin, iterator end);
	void remove(std::size_t begin, std::size_t end);
	void removeAt(std::size_t at);
	void removeNAt(std::size_t at, std::size_t n);
	void removeAfter(std::size_t from);

	void clear(void);

	std::size_t findNextIndex(WChar chr, std::size_t start = 0) const;
	std::size_t findPrevIndex(WChar chr, std::size_t start = 0) const;

	bool startsWith(WStringView with) const;

	constexpr WChar& operator[](std::size_t idx);
	constexpr const WChar& operator[](std::size_t idx) const;

	constexpr const_iterator begin(void) const;
	constexpr const_iterator end(void) const;

	constexpr iterator begin(void);
	constexpr iterator end(void);
};

constexpr WString operator""_ws(const char* str, std::size_t length) {
	return WString(std::string_view(str, length));
}

WString operator+(const WString& lhs, const WString& rhs);
WString operator+(const WString& lhs, const char* rhs);
WString operator+(const char* lhs, const WString& rhs);
WString operator+(const WString& lhs, WChar rhs);
WString operator+(WChar lhs, const WString& rhs);

class WStringView {
	const WChar* mData;
	std::size_t mLength;

public:
	using const_iterator = std::vector<WChar>::const_iterator;

	static const std::size_t npos = std::size_t(-1);

	constexpr WStringView(void);
	constexpr WStringView(const WStringView& string);
	constexpr WStringView(const WString& string);
	template <class IT>
	constexpr WStringView(IT begin, IT end);
	constexpr WStringView(std::nullptr_t) = delete;

	WStringView& operator=(const WStringView& other);

	constexpr std::size_t length(void) const;
	constexpr const WChar* data(void) const;

	constexpr const WChar& at(std::size_t idx) const;
	constexpr const WChar& operator[](std::size_t idx) const;

	constexpr const WChar& front(void) const;
	constexpr const WChar& back(void) const;

	constexpr void removePrefix(std::size_t count);
	constexpr void removeSuffix(std::size_t count);

	std::size_t findSubString(WStringView find, std::size_t start = 0) const;
	std::size_t findSubStringRtl(WStringView find, std::size_t start = WStringView::npos) const;

	constexpr bool startsWith(WStringView with) const;

	const WStringView subString(std::size_t cstart, std::size_t cend) const;

	constexpr const WChar* begin(void) const;
	constexpr const WChar* end(void) const;

	bool operator==(std::string_view other) const;
	bool operator==(WStringView other) const;
};

#include "impl/wstring.ipp"
#include "impl/wstringview.ipp"

#endif
