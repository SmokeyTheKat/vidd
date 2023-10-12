#include <vidd/utils.hpp>

#include <cctype>
#include <cstdlib>

namespace Utils {

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
