#include <vidd/fuzzywindow.hpp>

#include <vidd/vidd.hpp>
#include <vidd/log.hpp>

#include <algorithm>

namespace {

std::vector<std::string> fuzzyFind(std::vector<std::string_view> finds, const std::vector<std::string>& data) {
	std::vector<std::string> matches;
	for (const std::string& str : data) {
		bool found = true;
		for (const std::string_view& find : finds) {
			if (str.find(find) == std::string::npos) {
				found = false;
				break;
			}
		}
		if (found == true) {
			matches.push_back(str);
		}
	}
	return matches;
}

std::vector<std::string_view> splitAtSpaces(std::string_view text) {
	std::vector<std::string_view> words;

	std::size_t start = 0;
	for (std::size_t i = 0; i < text.length(); i++) {
		if (text[i] == ' ') {
			words.push_back(text.substr(start, i - start));
			start = i + 1;
		}
	}
	words.push_back(text.substr(start, text.length()));

	return words;
}

}; //namespace

FuzzyWindow::FuzzyWindow(
	const std::string& title,
	const std::vector<std::string>& data,
	Vec2 size,
	std::function<void(FuzzyWindow*,std::string)> callback
)
: Component(Vec2::zero(), size),
  mTitle(title),
  mData(data),
  mCallback(callback) {
	setSelectable(true);
	setLayer(1);
	calculateResults();
	mPrompt = TextPrompt({
		.change = [this] {
			mView = 0;
			mCursor = 0;
			calculateResults();
		},
		.submit = [this] {
			Log::log(std::to_string(mCursor));
			if (mResults.size() > 0) submit(mResults[mCursor]);
			else submit("");
		},
		.exit = [this] {
			submit("");
		}
	});
}

void FuzzyWindow::submit(const std::string& result) {
	if (mIsClosing == false) {
		mIsClosing = true;
		mCallback(this, result);
	}
}

Vec2 FuzzyWindow::getCursor(void) {
	Terminal::setCursor(Terminal::CursorStyle::SteadyBar);
	return getRealPos(Vec2(mPrompt.getCursor() + 1, 1));
}

void FuzzyWindow::onResize(void) {
	Vec2 parentSize = getParent()->getSize();
	Vec2 pos(
		(parentSize.x - mSize.x) / 2,
		(parentSize.y - mSize.y) / 2
	);
	setPos(pos);
}

void FuzzyWindow::onAttach(void) {
	onResize();
}

void FuzzyWindow::calculateResults(void) {
	mResults = fuzzyFind(splitAtSpaces(mPrompt.get()), mData);
	mCursor = std::clamp(mCursor, 0, (int)mResults.size() - 1);
	requireRedraw();
}

void FuzzyWindow::onDeselect(void) {
	submit("");
}

void FuzzyWindow::onLeftMouseButtonDown(Vec2 pos) {
	pos -= Vec2(1, 2);
	mCursor = mView + pos.y;
	requireRedraw();
}

void FuzzyWindow::onLeftMouseButtonDoubleDown(Vec2 pos) {
	pos -= Vec2(1, 2);
	mCursor = mView + pos.y;
	mPrompt.submit();
}

void FuzzyWindow::onKeyDown(Key key) {
	switch (key) {
	case Keys::Down:
	case Keys::Tab: {
		mCursor = std::min((int)mResults.size() - 1, mCursor + 1);
		if (mCursor >= mView + mSize.y - 3) mView += 1;
		requireRedraw();
	} break;
	case Keys::ScrollDown: {
		if (mView + 1 >= (int)mResults.size()) break;
		mView += 1;
		if (mCursor < mView) mCursor = mView;
		requireRedraw();
	} break;
	case Keys::ScrollUp: {
		if (mView == 0) break;
		mView -= 1;
		if (mCursor >= mView + mSize.y - 3) mCursor = mView + mSize.y - 4;
		requireRedraw();
	} break;
	case Keys::Up:
	case Keys::ShiftTab: {
		mCursor = std::max(0, mCursor - 1);
		if (mCursor < mView) mView = mCursor;
		requireRedraw();
	} break;
	default: {
		mPrompt.input(key);
	} break;
	}
}

void FuzzyWindow::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(Style(theme->text.fg, theme->text.bg));
	drawFilledBox(Vec2::zero(), mSize, ' ');
	drawBox(Vec2::zero(), mSize, Draw::normalLine);
	drawText(Vec2((mSize.x - mTitle.length()) / 2, 0), WString(mTitle));
	int y = 1;
	drawText(Vec2(1, y++), WString(mPrompt.get()));
	for (int i = mView; i < (int)mResults.size() && y < mSize.y - 1; i++) {
		if (i == mCursor) {
			Draw::style(Style(theme->text.bg, theme->text.fg));
		} else {
			Draw::style(Style(theme->text.fg, theme->text.bg));
		}
		drawText(Vec2(1, y++), WString(mResults[i]));
	}
}
