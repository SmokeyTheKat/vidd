#ifndef __VIDD_SYNTAX_HPP__
#define __VIDD_SYNTAX_HPP__

#include <map>
#include <string_view>
#include <vector>
#include <utility>

enum class StyleType {
	KeyWord = 0,
	Operator,
	Macro,
	Type,
	Comment,
	String,
	Const,
	Number,
	Function,
	StyleTypeLength,
};

struct Syntax {
	struct Range {
		enum class Type {
			Until = 0,
			UntilLineEnd,
			While,
		};
		Type type;
		bool inclusive;
		std::string_view from;
		std::string_view chars;
	};

	bool highlightTypes;
	bool highlightConsts;
	bool highlightFunctionCalls;
	bool functionCallsAnyBrackets;
	bool highlightNumbers;
	std::vector<std::string_view> stringSymbols;
	std::vector<std::pair<std::string_view, std::string_view>> multiStringSymbols;
	std::vector<std::pair<std::string_view, std::string_view>> multiLineCommentSymbols;
	std::vector<std::string_view> singleLineCommentSymbols;
	std::vector<std::string_view> operators;
	std::vector<std::tuple<Range, StyleType>> lineRanges;
	std::map<std::string_view, StyleType> wordStyles;
};

#endif
