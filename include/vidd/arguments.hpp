#ifndef __VIDD_ARGUMENTS_HPP__
#define __VIDD_ARGUMENTS_HPP__

#include <string>
#include <vector>
#include <algorithm>
#include <utility>

class Arguments {
	std::vector<std::string> mFlags;
	std::vector<std::string> mFiles;
	std::vector<std::pair<std::string, std::string>> mValues;

public:
	Arguments(int argc, char** argv);

	const std::vector<std::string>& getFiles(void) { return mFiles; };

	bool hasFlag(std::string_view flag) const {
		return std::find(mFlags.begin(), mFlags.end(), flag) != mFlags.end();
	};

	bool hasValue(std::string_view name) const {
		return std::find_if(
			mValues.begin(), mValues.end(),
			[name](const auto& value) { return value.first == name; }
		) != mValues.end();
	};

	std::string getValue(std::string_view name) const {
		auto it = std::find_if(
			mValues.begin(), mValues.end(),
			[name](const auto& value) { return value.first == name; }
		);
		return it == mValues.end() ? std::string("") : it->second;
	};

private:
	void parse(const std::vector<std::string>& args);
};

#endif