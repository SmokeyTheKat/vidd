#include <vidd/arguments.hpp>

#include <variant>
#include <cstring>

namespace {

struct ArgumentInfo {
	std::string_view name;
	std::string_view valueType;
	std::string_view description;
};

std::vector<ArgumentInfo> argumentInfo {
	{ "--",                "",       "every argument after this is a file" },
	{ "-",                 "",       "reads data from stdin into a buffer" },
	{ "--test-getch",      "",       "" },
};

const ArgumentInfo* getArgumentInfo(std::string_view name) {
	auto it = std::find_if(
		argumentInfo.begin(), argumentInfo.end(),
		[name](const ArgumentInfo& arg) { return arg.name == name; }
	);
	return it == argumentInfo.end() ? nullptr : &*it;
}

}; // anon namespace

Arguments::Arguments(int argc, char** argv) {
	std::vector<std::string> args;
	for (int i = 1; i < argc; i++) {
		args.emplace_back(argv[i]);
	}
	parse(args);
}

void Arguments::parse(const std::vector<std::string>& args) {
	bool onlyFilesLeft = false;
	for (std::size_t i = 0; i < args.size(); i++) {
		if (onlyFilesLeft) {
			mFiles.push_back(args[i]);
			continue;
		}

		if (args[i] == "--") {
			onlyFilesLeft = true;
		} else if (args[i][0] == '-') {
			const ArgumentInfo* arg = getArgumentInfo(args[i]);
			if (arg == nullptr || arg->valueType.length() == 0) {
				mFlags.push_back(args[i]);
			} else {
				mValues.push_back(std::make_pair(args[i], args[i + 1]));
				i += 1;
			}
		} else {
			mFiles.push_back(args[i]);
		}
	}

	if (mFiles.size() == 0 && !hasFlag("-")) {
		mFiles.push_back(PREFIX "/share/vidd/default");
	}
}

