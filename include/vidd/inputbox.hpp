#ifndef __VIDD_INPUTBOX_HPP__
#define __VIDD_INPUTBOX_HPP__

#include <vidd/component.hpp>
#include <vidd/textprompt.hpp>

#include <string>
#include <functional>

class InputBox : public Component {
public:
	using Callback = std::function<void(InputBox*, std::string)>;

private:
	std::string mTitle;
	TextPrompt mPrompt;
	Callback mCallback;
	bool mIsClosing = false;

public:
	InputBox(const std::string& title, Callback callback);

	void submit(const std::string& value);

	Vec2 getCursor(void) override;
	void onResize(void) override;
	void onAttach(void) override;
	void onDeselect(void) override;
	void onKeyDown(Key key) override;
	void render(void) override;
};

#endif
