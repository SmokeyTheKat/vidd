#ifndef __VIDD_SYNTAXER_HPP__
#define __VIDD_SYNTAXER_HPP__

#include <vidd/wchar.hpp>
#include <vidd/style.hpp>
#include <vidd/syntax.hpp>
#include <vidd/charsets.hpp>

#include <map>
#include <string_view>
#include <array>

struct Word {
	WStringView word;
	Style style;
};

struct Language;
struct Theme;

class Syntaxer {
	enum class State {
		String,
		MultiLineString,
		Comment,
		Global
	};

	enum class TokenType {
		Word,
		Number,
		LineRange,
		String,
		Comment,
		Operator,
		None,
	};
	
	struct Token {
		WStringView word;
		TokenType type;
	};

	State mState = State::Global;
	WStringView mStringOpener;
	const Language* mLang;
	const Theme* mTheme;
	
public:
	Syntaxer(const Language* lang, const Theme* theme)
	: mLang(lang), mTheme(theme) {};

	void skimState(WStringView line);
	std::vector<Word> highlight(WStringView line);

private:
	std::vector<Token> tokenize(WStringView line);
	void skimTokenize(WStringView line);
};


#endif
