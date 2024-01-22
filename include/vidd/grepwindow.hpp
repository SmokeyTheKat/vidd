#ifndef __VIDD_GREPWINDOW_HPP__
#define __VIDD_GREPWINDOW_HPP__

#include <vidd/component.hpp>
#include <vidd/textprompt.hpp>

#include <string>
#include <string_view>
#include <functional>
#include <vector>

class GrepWindow : public Component {
	struct Result {
		int x = -1;
		int y = -1;
		std::string file = "";
		std::string line = "";
	};

	std::function<void(GrepWindow*, Result)> mCallback;
	std::vector<Result> mResults;
	TextPrompt mPrompt;
	int mCursor = 0;
	int mView = 0;
	bool mIsClosing = false;

public:
	GrepWindow(
		Vec2 size,
		std::function<void(GrepWindow*, Result)> callback
	);

	void submit(const Result& result);

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
