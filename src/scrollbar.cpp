#include <vidd/scrollbar.hpp>

#include <vidd/vidd.hpp>

#include <cmath>
#include <algorithm>

ScrollBar::ScrollBar(Side side, CalculateSizeCallback cs, OnChangeCallback oc)
: mSide(side), mCalculateSize(cs), mOnChange(oc), mDetails({0.0, 1.0}) {
}

std::pair<int, int> ScrollBar::getScrollBarPosAndSize(void) {
	int pos = mDetails.pos * mSize.y;
	int size = std::max(1, (int)(mDetails.size * mSize.y));

	if (pos + size > mSize.y) {
		pos = mSize.y - size;
	}

	return std::make_pair(pos, size);
}

void ScrollBar::onPrerender(void) {
	SizeDetails dets = mCalculateSize();
	if (mDetails != dets) {
		mDetails = dets;
		requireRedraw();
	}

	setSizeQuiet(Vec2(1, getParent()->getSize().y - 1));
	setPosQuiet(
		mSide == Side::Left
			? Vec2(0, 0)
			: Vec2(getParent()->getSize().x - 1, 0)
	);
}

#define SCROLLBAR_CHARACTER "│"

void ScrollBar::render(void) {
	const Theme* theme = Vidd::getTheme();
	auto [pos, size] = getScrollBarPosAndSize();

	Draw::style(Style(Color(120), theme->bg.bg));
	drawVLine(Vec2(0, 0), mSize.y, SCROLLBAR_CHARACTER);

	Draw::style(Style(Color(255), theme->bg.bg));
	drawVLine(Vec2(0, pos), size, SCROLLBAR_CHARACTER);
}
