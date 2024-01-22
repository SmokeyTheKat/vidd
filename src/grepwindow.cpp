#include <vidd/grepwindow.hpp>

#include <vidd/vidd.hpp>
#include <vidd/procstream.hpp>
#include <vidd/log.hpp>

#include <algorithm>
#include <stdio.h>

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

std::vector<std::string> readAllLinesFp(FILE* fp) {
		std::vector<std::string> out;
		char buf[512];
		std::string cur;
		while (true) {
			int length = std::fread(buf, 1, sizeof(buf), fp);
			if (length <= 0) break;
			std::string_view chunk(buf, length);
			for (std::size_t i = 0; i < chunk.length(); i++) {
				if (chunk[i] == '\n') {
					out.push_back(std::move(cur));
					cur.clear();
				} else if (Utils::isSpace(chunk[i])) {
					cur += ' ';
				} else {
					cur += chunk[i];
				}
			}
		}
		out.push_back(cur);
		return out;
}

}; //namespace

GrepWindow::GrepWindow(
	Vec2 size,
	std::function<void(GrepWindow*, Result)> callback
)
: Component(Vec2::zero(), size),
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
			if (mResults.size() > 0) submit(mResults[mCursor]);
			else submit(Result());
		},
		.exit = [this] {
			submit(Result());
		}
	});
}

void GrepWindow::submit(const Result& result) {
	if (mIsClosing == false) {
		mIsClosing = true;
		mCallback(this, result);
	}
}

Vec2 GrepWindow::getCursor(void) {
	Terminal::setCursor(Terminal::CursorStyle::SteadyBar);
	return getRealPos(Vec2(mPrompt.getCursor() + 1, 1));
}

void GrepWindow::onResize(void) {
	Vec2 parentSize = getParent()->getSize();
	Vec2 pos(
		(parentSize.x - mSize.x) / 2,
		(parentSize.y - mSize.y) / 2
	);
	setPos(pos);
}

void GrepWindow::onAttach(void) {
	onResize();
}

void GrepWindow::calculateResults(void) {
	requireRedraw();
	mResults.clear();
	if (mPrompt.get().length() == 0) {
		mCursor = 0;
		return;
	}

	std::vector<std::string_view> finds = Utils::splitAtSpaces(mPrompt.get());

	std::string com = (std::string("rg -n --column '") + Utils::joinStrings(finds, "' | rg '") + "' 2>&1");
	FILE* fp = ::popen(com.c_str(), "r");
	std::vector<std::string> lines = readAllLinesFp(fp);
	::pclose(fp);
	for (const std::string& line : lines) {
		if (line.length() == 0) continue;
		std::vector<std::string_view> splits = Utils::splitAtFirst(line, ':', 3);
		if (splits.size() != 4) continue;
		Result r = {
			Utils::stringToInt(splits[2]),
			Utils::stringToInt(splits[1]),
			std::string(splits[0]),
			std::string(splits[3]),
		};
		mResults.push_back(r);
	}

	mCursor = std::clamp(mCursor, 0, (int)mResults.size() - 1);
}

void GrepWindow::onDeselect(void) {
	submit(Result());
}

void GrepWindow::onLeftMouseButtonDown(Vec2 pos) {
	pos -= Vec2(1, 2);
	mCursor = mView + pos.y;
	requireRedraw();
}

void GrepWindow::onLeftMouseButtonDoubleDown(Vec2 pos) {
	pos -= Vec2(1, 2);
	mCursor = mView + pos.y;
	mPrompt.submit();
}

void GrepWindow::onKeyDown(Key key) {
	switch (key) {
	case Keys::Tab: {
		mCursor = std::min((int)mResults.size() - 1, mCursor + 1);
		if (mCursor >= mView + (mSize.y - 2) / 2 - 1) {
			mView += 1;
		}
		requireRedraw();
	} break;
	case Keys::ShiftTab: {
		mCursor = std::max(0, mCursor - 1);
		if (mCursor < mView) {
			mView = mCursor;
		}
		requireRedraw();
	} break;
	case Keys::Down:
	case Keys::ScrollDown: {
		if (mView + 1 >= (int)mResults.size()) break;
		mView += 1;
		if (mCursor < mView) mCursor = mView;
		requireRedraw();
	} break;
	case Keys::Up:
	case Keys::ScrollUp: {
		if (mView == 0) break;
		mView -= 1;
		if (mCursor >= mView + mSize.y - 3) mCursor = mView + mSize.y - 4;
		requireRedraw();
	} break;
	default: {
		mPrompt.input(key);
	} break;
	}
}

void GrepWindow::render(void) {
	const WString title = "grep"_ws;
	const Theme* theme = Vidd::getTheme();
	Draw::style(Style(theme->text.fg, theme->text.bg));
	drawFilledBox(Vec2::zero(), mSize, ' ');
	drawBox(Vec2::zero(), mSize, Draw::normalLine);
	drawText(Vec2((mSize.x - title.length()) / 2, 0), title);
	int y = 1;
	drawText(Vec2(1, y++), WString(mPrompt.get()));
	for (int i = mView; i < (int)mResults.size() && y + 1 < mSize.y - 1; i++) {
		Style style = Style(theme->text.fg, theme->text.bg);
		if (i == mCursor) {
			style += theme->highlight;
		}
		Draw::style(style);
		drawText(Vec2(1, y++), WString(
			mResults[i].file + ":" +
			std::to_string(mResults[i].y)
		));

		style.fg = theme->important;
		Draw::style(style);
		drawText(Vec2(1, y++), WString(
			mResults[i].line
		));
	}
}
