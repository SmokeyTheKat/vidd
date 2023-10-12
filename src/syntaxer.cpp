#include <vidd/syntaxer.hpp>

#include <vidd/language.hpp>
#include <vidd/theme.hpp>

namespace {

bool isAllUpper(WStringView str) {
	for (auto c : str) {
		if (CharSets::letters.contains(c) && std::isupper(c) == false) {
			return false;
		}
	}
	return true;
}

}; // namespace

std::vector<Syntaxer::Token> Syntaxer::tokenize(WStringView line) {
	std::vector<Token> tokens;
	std::size_t i = 0;

	auto isAtStringSymbol = [this, &i, &line](void) -> std::optional<std::string_view> {
		if (i >= line.length()) return {};
		for (const auto& ss : mLang->syntax.stringSymbols) {
			if (
				i + ss.length() <= line.length() &&
				line.subString(i, i + ss.length()) == ss &&
				!(i > 0 && line[i-1] == '\\' && (i <= 1 || line[i-2] != '\\'))
			) return ss;
		}
		return {};
	};

	auto isAtMatchingStringSymbol = [this, isAtStringSymbol](void) -> bool {
		if (auto opt = isAtStringSymbol()) {
			return *opt == mStringOpener;
		}
		return false;
	};

	auto isAtLineRange = [this, &i, &line](void) -> std::optional<std::reference_wrapper<const decltype(mLang->syntax.lineRanges)::value_type>> {
		if (i >= line.length()) return {};
		for (const auto& lrp : mLang->syntax.lineRanges) {
			const auto& range = std::get<0>(lrp);
			if (
				i + range.from.length() <= line.length() &&
				line.subString(i, i + range.from.length()) == range.from
			) {
				switch (range.type) {
				case Syntax::Range::Type::While: {
					if (!(
						i + range.from.length() < line.length() &&
						range.chars.find(line[i + range.from.length()]) != std::string_view::npos
					)) {
						return {};
					}
				} break;
				default: break;
				}
				return std::cref(lrp);
			}
		}
		return {};
	};

	auto isAtMultiLineCommentBegin = [this, &i, &line](void) -> std::optional<std::string_view> {
		if (i >= line.length()) return {};
		for (const auto& ml : mLang->syntax.multiLineCommentSymbols) {
			const auto& st = std::get<0>(ml);
			if (
				i + st.length() <= line.length() &&
				line.subString(i, i + st.length()) == st
			) return st;
		}
		return {};
	};

	auto isAtMultiLineCommentEnd = [this, &i, &line](void) -> std::optional<std::string_view> {
		if (i >= line.length()) return {};
		for (const auto& ml : mLang->syntax.multiLineCommentSymbols) {
			const auto& se = std::get<1>(ml);
			if (
				i + se.length() <= line.length() &&
				line.subString(i, i + se.length()) == se
			) return se;
		}
		return {};
	};

	auto isAtSingleLineComment = [this, &i, &line](void) -> std::optional<std::string_view> {
		if (i >= line.length()) return {};
		for (const auto& sl : mLang->syntax.singleLineCommentSymbols) {
			if (
				i + sl.length() <= line.length() &&
				line.subString(i, i + sl.length()) == sl
			) return sl;
		}
		return {};
	};

	auto isAtOperator = [this, &i, &line](void) -> std::optional<std::string_view> {
		if (i >= line.length()) return {};
		for (const auto& op : mLang->syntax.operators) {
			if (
				i + op.length() <= line.length() &&
				line.subString(i, i + op.length()) == op
			) return op;
		}
		return {};
	};


	while (i < line.length()) {
		if (mState == State::Global) {
			if (auto opt = isAtStringSymbol()) {
				std::string_view ss = *opt;
				WStringView stringSymbol = line.subString(i, i + ss.length());
				i += ss.length();
				tokens.push_back(Token{ stringSymbol, TokenType::String });
				mState = State::String;
				mStringOpener = stringSymbol;
			} else if (auto opt = isAtMultiLineCommentBegin()) {
				std::string_view st = *opt;
				tokens.push_back(Token{ line.subString(i, i + st.length()), TokenType::Comment });
				i += st.length();
				mState = State::Comment;
			} else if (auto opt = isAtLineRange()) {
				const auto& lrp = opt->get();
				const auto& range = std::get<0>(lrp);
				std::size_t end = i + range.from.length();
				switch (range.type) {
				case Syntax::Range::Type::UntilLineEnd: {
					end = line.length();
					tokens.push_back(Token{ line.subString(i, end), TokenType::LineRange });
				} break;
				case Syntax::Range::Type::Until: {
					while (end < line.length() && range.chars.find(line[end]) == std::string_view::npos) end++;
					if (range.inclusive == true) {
						end += 1;
					}
					tokens.push_back(Token{ line.subString(i, end), TokenType::LineRange });
				} break;
				case Syntax::Range::Type::While: {
					while (end < line.length() && range.chars.find(line[end]) != std::string_view::npos) end++;
					tokens.push_back(Token{ line.subString(i, end), TokenType::LineRange });
				} break;
				}
				i = end;
			} else if (isAtSingleLineComment()) {
				tokens.push_back(Token{ line.subString(i, line.length()), TokenType::Comment });
				i = line.length();
			} else if (auto opt = isAtOperator()) {
				std::string_view op = *opt;
				tokens.push_back(Token{ line.subString(i, i + op.length()), TokenType::Operator });
				i += op.length();
			} else if (CharSets::numbers.contains(line[i])) {
				std::size_t start = i;
				while (i < line.length() && CharSets::numberCharacters.contains(line[i])) i++;
				tokens.push_back(Token{ line.subString(start, i), TokenType::Number });
			} else if (i < line.length() && CharSets::characters.contains(line[i])) {
				std::size_t start = i;
				while (i < line.length() && CharSets::characters.contains(line[i])) i++;
				tokens.push_back(Token{ line.subString(start, i), TokenType::Word });
			} else if (i < line.length() && CharSets::whitespace.contains(line[i])) {
				std::size_t start = i;
				while (i < line.length() && CharSets::whitespace.contains(line[i])) i++;
				tokens.push_back(Token{ line.subString(start, i), TokenType::None });
			} else {
				tokens.push_back(Token{ line.subString(i, i + 1), TokenType::None });
				i++;
			}
		} else if (mState == State::String) {
			int start = i;
			while (i < line.length() && !isAtMatchingStringSymbol()) {
				i++;
			}

			if (auto opt = isAtStringSymbol()) {
				std::string_view ss = *opt;
				i += ss.length();
				tokens.push_back(Token{ line.subString(start, i), TokenType::String });
				mState = State::Global;
				continue;
			} else {
				tokens.push_back(Token{ line.subString(start, i), TokenType::String });
			}
			continue;
		} else if (mState == State::Comment) {
			int start = i;
			while (i < line.length() && !isAtMultiLineCommentEnd()) {
				i++;
			}

			if (auto opt = isAtMultiLineCommentEnd()) {
				std::string_view se = *opt;
				i += se.length();
				tokens.push_back(Token{ line.subString(start, i), TokenType::Comment });
				mState = State::Global;
				continue;
			} else {
				tokens.push_back(Token{ line.subString(start, i), TokenType::Comment });
			}
		}
	}

	return tokens;
}


void Syntaxer::skimState(WStringView line) {
	tokenize(line);
}

std::vector<Word> Syntaxer::highlight(WStringView line) {
	std::vector<Token> tokens = tokenize(line);
	std::vector<Word> words;

	auto isAtFunctionCall = [&tokens](std::size_t i) {
		if (tokens[i].type != TokenType::Word) return false;
		if (i + 1 >= tokens.size()) return false;
		if (tokens[i + 1].word[0] == '(') return true;
		if (tokens[i + 1].word[0] == '<') {
			std::size_t j = i + 2;
			while (j < tokens.size() && tokens[j].word[0] != '>') j++;
			if (j + 1 >= tokens.size()) return false;
			return tokens[j + 1].word[0] == '(';
		}
		return false;
	};

	for (std::size_t i = 0; i < tokens.size(); i++) {
		switch (Token& token = tokens[i]; token.type) {
		case TokenType::Word: {
			std::string word = WString(token.word).string();
			if (mLang->syntax.wordStyles.find(word) != mLang->syntax.wordStyles.end()) {
				words.push_back(Word{ token.word, mTheme->getSyntaxStyle(mLang->syntax.wordStyles.at(word)) });
			} else {
				if (mLang->syntax.highlightFunctionCalls && isAtFunctionCall(i)) {
					words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::Function) });
				} else if (mLang->syntax.highlightConsts && isAllUpper(token.word)) {
					words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::Const) });
				} else if (mLang->syntax.highlightTypes && std::isupper(token.word[0])) {
					words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::Type) });
				} else if (mLang->syntax.highlightTypes && token.word.length() > 2 && token.word.subString(token.word.length() - 2, token.word.length()) == "_t") {
					words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::Type) });
				} else {
					words.push_back(Word{ token.word, mTheme->text });
				}
			}
		} break;

		case TokenType::LineRange: {
			for (const auto& lr : mLang->syntax.lineRanges) {
				const auto& range = std::get<0>(lr);
				if (token.word.subString(0, range.from.length()) == range.from) {
					words.push_back(Word{ tokens[i].word, mTheme->getSyntaxStyle(std::get<1>(lr)) });
					break;
				}
			}
		} break;

		case TokenType::String: {
			words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::String) });
		} break;

		case TokenType::Comment: {
			words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::Comment) });
		} break;

		case TokenType::Operator: {
			words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::Operator) });
		} break;

		case TokenType::Number: {
			if (mLang->syntax.highlightNumbers) {
				words.push_back(Word{ token.word, mTheme->getSyntaxStyle(StyleType::Number) });
			} else {
				words.push_back(Word{ token.word, mTheme->text });
			}
		} break;

		case TokenType::None:
		default: {
			words.push_back(Word{ token.word, mTheme->text });
		} break;
		}
	}
	return words;
}
