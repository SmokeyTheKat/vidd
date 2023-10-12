#include <vidd/logclient.hpp>

#include <vidd/tab.hpp>
#include <vidd/log.hpp>

void LogClient::onKeyDown(Key key) {
	if (key == Keys::ctrl('h')) {
		getTab()->incrementSplitPos(-0.05);
	} else if (key == Keys::ctrl('l')) {
		getTab()->incrementSplitPos(0.05);
	} else if (key == Keys::ctrl('j')) {
		getTab()->selectNextClient();
	} else if (key == Keys::ctrl('k')) {
		getTab()->selectPrevClient();
	} else if (key == Keys::ctrl('i')) {
		getTab()->incrementMasterCount(1);
	} else if (key == Keys::ctrl('d')) {
		getTab()->incrementMasterCount(-1);
	} else if (key == Keys::ctrl('c')) {
		close();
	} else if (key == Keys::Return) {
		getTab()->swapSelectedWithMaster();
	}
}

void LogClient::onPrerender(void) {
	if (mLastSize != Log::get().size()) {
		requireRedraw();
		mLastSize = Log::get().size();
	}
}

void LogClient::render(void) {
	auto& data = Log::get();
	int y = 0;
	Draw::style(Style(Color(255), Color(0)));
	drawFilledBox(Vec2(0, 0), mSize, " ");
	for (int i = std::max(0, ((int)data.size()) - mSize.y); i < (int)data.size(); i++) {
		drawText(Vec2(0, y++), WString(data[i]));
	}
}
