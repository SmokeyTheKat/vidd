#ifndef __VIDD_FUZZYWINDOW_HPP__
#define __VIDD_FUZZYWINDOW_HPP__

#include <vidd/component.hpp>
#include <vidd/textprompt.hpp>

#include <string>
#include <string_view>
#include <functional>
#include <vector>

class FuzzyWindow : public Component {
	std::string mTitle;
	std::vector<std::string> mData;
	std::vector<std::string> mResults;
	std::function<void(FuzzyWindow*,std::string)> mCallback;
	TextPrompt mPrompt;
	int mCursor = 0;
	int mView = 0;
	bool mIsClosing = false;

public:
	FuzzyWindow(
		const std::string& title,
		const std::vector<std::string>& data,
		Vec2 size,
		std::function<void(FuzzyWindow*,std::string)> callback
	);

	void submit(const std::string& result);

	Vec2 getCursor(void) override;
	void onResize(void) override;
	void onAttach(void) override;
	void onDeselect(void) override;
	void onLeftMouseButtonDown(Vec2 pos) override;
	void onLeftMouseButtonDoubleDown(Vec2 pos) override;
	void onKeyDown(Key key) override;
	void render(void) override;

private:
	void calculateResults(void);
};

#endif
