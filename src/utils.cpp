#include <vidd/utils.hpp>

#include <cctype>
#include <cstdlib>
#include <filesystem>

namespace Utils {

std::string getPwd(void) {
	return std::filesystem::current_path();
}

std::vector<std::string_view> splitAt(std::string_view text, char at) {
	std::vector<std::string_view> words;

	std::size_t start = 0;
	for (std::size_t i = 0; i < text.length(); i++) {
		if (text[i] == at) {
			words.push_back(text.substr(start, i - start));
			start = i + 1;
		}
	}
	words.push_back(text.substr(start, text.length()));

	return words;
}

std::vector<WStringView> splitAt(WStringView text, WChar at) {
	std::vector<WStringView> words;

	std::size_t start = 0;
	for (std::size_t i = 0; i < text.length(); i++) {
		if (text[i] == at) {
			words.push_back(text.subString(start, i));
			start = i + 1;
		}
	}
	words.push_back(text.subString(start, text.length()));

	return words;
}

std::vector<std::string_view> splitAtFirst(std::string_view text, char at, int n) {
	std::vector<std::string_view> words;

	int c = 0;
	std::size_t start = 0;
	for (std::size_t i = 0; i < text.length(); i++) {
		if (text[i] == at && c < n) {
			c++;
			words.push_back(text.substr(start, i - start));
			start = i + 1;
		}
	}
	words.push_back(text.substr(start, text.length()));

	return words;
}

std::vector<std::string_view> splitAtSpaces(std::string_view text) {
	return splitAt(text, ' ');
}

void stringReplace(std::string& string, std::string_view find, std::string_view replace) {
	size_t pos = 0;
	while ((pos = string.find(find, pos)) != std::string::npos) {
		string.replace(pos, find.length(), replace);
		pos += replace.length();
	}
}

std::string getPathBaseName(std::string path) {
	return path.substr(path.find_last_of("/\\") + 1);
}

std::string repeatString(std::string_view str, int n) {
	std::string out;
	out.reserve(n * str.length());
	for (int i = 0; i < n; i++) {
		out.append(str);
	}
	return out;
}

bool stringIsUppercase(std::string_view str) {
	for (auto c : str) {
		if (std::isalpha(c) && !std::isupper(c)) return false;
	}
	return true;
}

}; // namepsace Utils
