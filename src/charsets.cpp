#include <vidd/charsets.hpp>

const CharSet CharSets::getCharSetOf(char c) {
	if (CharSets::characters.contains(c)) {
		return CharSets::characters;
	} else if (CharSets::whitespace.contains(c)) {
		return CharSets::whitespace;
	} else if (CharSets::specials.contains(c)) {
		return CharSets::specials;
	} else {
		return CharSets::all;
	}
};
