#ifndef __VIDD_CODEVIEW_HPP__
#define __VIDD_CODEVIEW_HPP__

#include <vidd/component.hpp>

class TextEditor;

class CodeView : public Component {
	const TextEditor* mEditor;
	std::vector<std::pair<std::string, Vec2>> mJumpModeWords;

public:
	CodeView(const TextEditor* editor);

	void enterJumpMode(const std::vector<std::pair<std::string, Vec2>>& words);
	void exitJumpMode(void);

	void onPrerender(void) override;
	void render(void) override;
};

#endif
