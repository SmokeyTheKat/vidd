#include <vidd/textloop.hpp>

#include <vidd/vidd.hpp>

LoopText::LoopText(WString text, int size)
: Component(Vec2::zero(), Vec2(size, 1)), mText(text), mPos(0) {
}

void LoopText::onPrerender(void) {
	requireSelfRedraw();
}

void LoopText::render(void) {
	static int tick = 0;
	const Theme* theme = Vidd::getTheme();
	Draw::style(theme->topBarActive);

	drawText(Vec2::zero(), WStringView(mText).subString(mPos, mText.length()));
	if (
		(mPos == 0 && tick == 15) ||
		(mPos >= mText.length() - mSize.x && tick == 15) ||
		(mPos > 0 && mPos < mText.length() - mSize.x && tick == 3)) {
		mPos += 1;
		if ((int)mText.length() - mPos < mSize.x) mPos = 0;
		tick = 0;
	} else tick++;
}

