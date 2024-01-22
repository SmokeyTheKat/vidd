#include <vidd/inputbox.hpp>

#include <vidd/vidd.hpp>

InputBox::InputBox(const std::string& title, Callback callback)
: Component(Vec2::zero(), Vec2(25, 3)), mTitle(title), mCallback(callback) {
	setSelectable(true);
	setLayer(1);
	mPrompt = TextPrompt({
		.change = []{},
		.submit = [this] { submit(mPrompt.get()); },
		.exit = [this] { submit(""); }
	});
}

void InputBox::submit(const std::string& value) {
	if (mIsClosing == false) {
		mIsClosing = true;
		mCallback(this, value);
	}
}

Vec2 InputBox::getCursor(void) {
	Terminal::setCursor(Terminal::CursorStyle::SteadyBar);
	return getRealPos(Vec2(mPrompt.getCursor() + 1, 1));
}

void InputBox::onResize(void) {
	Vec2 parentSize = getParent()->getSize();
	Vec2 pos(
		(parentSize.x - mSize.x) / 2,
		(parentSize.y - mSize.y) / 2
	);
	setPos(pos);
}

void InputBox::onAttach(void) {
	onResize();
}

void InputBox::onDeselect(void) {
	submit("");
}

void InputBox::onKeyDown(Key key) {
	requireRedraw();
	mPrompt.input(key);
}

void InputBox::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(Style(theme->text.fg, theme->text.bg));
	drawFilledBox(Vec2::zero(), mSize, ' ');
	drawBox(Vec2::zero(), mSize, Draw::normalLine);
	drawText(Vec2((mSize.x - mTitle.length()) / 2, 0), WString(mTitle));
	drawText(Vec2(1, 1), WString(mPrompt.get()));
}
