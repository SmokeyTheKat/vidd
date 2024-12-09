#include <vidd/mathboardclient.hpp>

#include <vidd/vidd.hpp>

MathBoardClient::MathBoardClient(Tab* tab)
: Client(tab) {
	setSelectable(true);
};

void MathBoardClient::render(void) {
	for (const Input& inp : mInputs) {
	}
	const Theme* theme = Vidd::getTheme();
	if (isSelected()) {
		Draw::style(theme->barActive);
	} else {
		Draw::style(theme->barInactive);
	}
	drawFilledBox(Vec2(0, mSize.y - 1), Vec2(mSize.x, 1), '#');
}

