#ifndef __VIDD_CHARSETS_HPP__
#define __VIDD_CHARSETS_HPP__

#include <string>
#include <string_view>

struct CharSet {
	std::string_view data;

	bool contains(char c) const {
		return data.find(c) != std::string_view::npos;
	};

	friend bool operator==(const CharSet& lhs, const CharSet& rhs) {
		return lhs.data == rhs.data;
	};

	friend bool operator!=(const CharSet& lhs, const CharSet& rhs) {
		return !(lhs == rhs);
	};

	operator std::string_view() const {
		return data;
	};
};

namespace CharSets {
	const CharSet all = { " \r\n\t\f\vabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_`~!@#$%^&*()-=+[]{}\\|;:'\",<.>/?\0" };
	const CharSet whitespace = { " \r\n\t\f\v\0" };
	const CharSet nonwhitespace = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_`~!@#$%^&*()-=+[]{}\\|;:'\",<.>/?\0" };
	const CharSet nonescape = { " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_`~!@#$%^&*()-=+[]{}\\|;:'\",<.>/?" };
	const CharSet letters = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" };
	const CharSet lowerLetters = { "abcdefghijklmnopqrstuvwxyz" };
	const CharSet upperLetters = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
	const CharSet characters = { "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_" };
	const CharSet numbers = { "0123456789" };
	const CharSet numberCharacters = { "0123456789.xXaAbBcCdDeEfF" };
	const CharSet specials = { "`~!@#$%^&*()-=+[]{}\\|;:'\",<.>/?" };
	const CharSet brackets = { "{}()<>[]" };
	const CharSet openBrackets = { "{(<[" };
	const CharSet closeBrackets = { "})>]" };
	const CharSet stringcharacters = { "\"'`" };

	constexpr char oppositeBracket(char c) {
		switch (c) {
		case '{': return '}';
		case '}': return '{';
		case '(': return ')';
		case ')': return '(';
		case '<': return '>';
		case '>': return '<';
		case '[': return ']';
		case ']': return '[';
		default: return c;
		}
	}

	const CharSet getCharSetOf(char c);
};

#endif
