#ifndef __VIDD_PARSE_STRING_IPP__
#define __VIDD_PARSE_STRING_IPP__

#include <vidd/utils.hpp>
#include <vidd/cstring.hpp>

constexpr ParseString::ParseString(const char* data)
: mOrigData(data), mData(data) {
	mOrigLength = CString::length(data);
	mLength = mOrigLength;
}

constexpr ParseString::ParseString(const char* data, std::size_t length)
: mOrigData(data), mOrigLength(length), mData(data), mLength(length) {};

constexpr ParseString::ParseString(std::string_view str)
: ParseString(str.data(), str.length()) {};

constexpr ParseString::ParseString(const std::string& str)
: ParseString(str.data(), str.length()) {};

constexpr ParseString::ParseString(const ParseString& other)
: ParseString(other.mData, other.mLength) {};

constexpr std::string ParseString::string(void) const {
	return std::string(mData, mLength);
}

constexpr std::string_view ParseString::view(void) const {
	return std::string_view(mData, mLength);
}

constexpr long ParseString::getInt(int base) const {
	return Utils::stringToInt(string(), base);
}

constexpr std::size_t ParseString::length(void) const {
	return mLength;
}

constexpr bool ParseString::isEmpty(void) const {
	return mLength == 0;
}

constexpr char ParseString::front(void) const {
	return mData[0];
}

constexpr char ParseString::back(void) const {
	return mData[mLength - 1];
}

constexpr ParseString ParseString::subString(std::size_t begin, std::size_t end) const {
	return ParseString(mData + begin, indexElse(end) - begin);
}

constexpr ParseString ParseString::pop(std::size_t count) {
	ParseString popped(mData, count);
	mData += count;
	mLength -= count;
	return popped;
}

constexpr ParseString ParseString::popBack(std::size_t count) {
	ParseString popped(mData + mLength - count, count);
	mLength -= count;
	return popped;
}

constexpr ParseString ParseString::popWhile(auto&& func) {
	std::size_t end = 0;
	while (end < mLength && func(mData[end])) end++;
	ParseString popped(mData, end);
	mData += end;
	mLength -= end;
	return popped;
}

constexpr ParseString ParseString::popBackWhile(auto&& func) {
	std::size_t end = 0;
	while (end < mLength && func(mData[mLength - 1 - end])) end++;
	ParseString popped(mData + mLength - end, end);
	mLength -= end;
	return popped;
}

constexpr ParseString ParseString::popUntil(char until) {
	std::size_t pos = indexElse(find(until));
	ParseString popped(mData, pos);
	mData += pos;
	mLength -= pos;
	return popped;
}

constexpr ParseString ParseString::popUntil(ParseString until) {
	std::size_t pos = indexElse(find(until));
	ParseString popped(mData, pos);
	mData += pos;
	mLength -= pos;
	return popped;
}

constexpr ParseString ParseString::popUntilAndSkip(char until) {
	ParseString popped = popUntil(until);
	if (mLength > 0) {
		mData += 1;
		mLength -= 1;
	}
	return popped;
}

constexpr ParseString ParseString::popUntilAndSkip(ParseString until) {
	ParseString popped = popUntil(until);
	if (mLength >= until.length()) {
		mData += until.length();
		mLength -= until.length();
	}
	return popped;
}

constexpr std::vector<ParseString> ParseString::split(char at) const {
	ParseString str(*this);
	std::vector<ParseString> splits;
	while (str.isEmpty() == false) {
		splits.push_back(str.popUntilAndSkip(at));
	}

	return splits;
}

constexpr std::vector<ParseString> ParseString::split(ParseString at) const {
	ParseString str(*this);
	std::vector<ParseString> splits;
	while (str.isEmpty() == false) {
		splits.push_back(str.popUntilAndSkip(at));
	}

	return splits;
}

constexpr ParseString ParseString::strip(void) const {
	ParseString str = *this;
	str.popWhile([](char c) { return Utils::isSpace(c); });
	str.popBackWhile([](char c) { return Utils::isSpace(c); });
	return str;
}

constexpr std::size_t ParseString::find(char chr) const {
	for (std::size_t i = 0; i < mLength; i++) {
		if (mData[i] == chr) return i;
	}
	return ParseString::npos;
}

constexpr std::size_t ParseString::find(ParseString str) const {
	for (std::size_t i = 0; i < mLength - str.length(); i++) {
		if (subString(i, i + str.length()) == str) return i;
	}
	return ParseString::npos;
}

constexpr std::size_t ParseString::parsedCount(void) const {
	(void)mOrigLength;
	return std::distance(mOrigData, mData);
}

constexpr char ParseString::operator[](int idx) const {
	return mData[idx];
}

constexpr char ParseString::operator*(void) const {
	return front();
}

constexpr bool ParseString::operator==(const char* other) const {
	std::string_view otherView(other);
	return view() == otherView;
}

constexpr bool ParseString::operator==(const ParseString& other) const {
	return view() == other.view();
}

constexpr bool ParseString::operator==(std::string_view other) const {
	return view() == other;
}

constexpr std::size_t ParseString::indexElse(std::size_t val) const {
	return val != ParseString::npos ? val : mLength;
}

#endif
