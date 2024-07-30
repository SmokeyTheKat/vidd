#ifndef __VIDD_VIDD_HPP__
#define __VIDD_VIDD_HPP__

#include <vidd/arguments.hpp>
#include <vidd/display.hpp>
#include <vidd/tabarea.hpp>
#include <vidd/theme.hpp>

class Vidd {
	Arguments mArgs;
	Display mDisplay;
	TabArea mTabArea;

	bool mShow80Line = true;

	const Theme* mTheme = nullptr;

public:
	static Vidd* instance;

	Vidd(const Arguments& args);

	int run(void);
	void quit(void);

	static bool hasFlag(std::string_view name) {
		return instance->mArgs.hasFlag(name);
	};

	static bool hasValueArg(std::string_view name) {
		return instance->mArgs.hasValue(name);
	};

	static std::string getValueArg(std::string_view name) {
		return instance->mArgs.getValue(name);
	};

	static void setTheme(const Theme* theme) { 
		if (instance == nullptr) return;
		instance->mTheme = theme;
		instance->shuffleDisplay();
	};

	static const Theme* getTheme(void) { 
		if (instance == nullptr) return nullptr;
		return instance->mTheme;
	};

	static bool getShow80Line(void) { 
		if (instance == nullptr) return true;
		return instance->mShow80Line;
	};

	static void setShow80Line(bool value) { 
		if (instance == nullptr) return;
		instance->mShow80Line = value;
	};

private:
	void shuffleDisplay(void);
};

#endif
