#include <vidd/terminalclient.hpp>

#include <vidd/tab.hpp>
#include <vidd/tabarea.hpp>

namespace {

#define TAB self->getTab()
#define TABAREA self->getTabArea()
#define CLIENT self

const AliasBinds aliases = {
	{ { Keys::CtrlSpace, Keys::CtrlSpace }, { Keys::CtrlSpace } },
};

const KeyBinds keyBinds = {
	KEYBIND(TerminalClient, ({ Keys::ctrl('h') }), TAB->incrementSplitPos(-0.05)) \
	KEYBIND(TerminalClient, ({ Keys::ctrl('l') }), TAB->incrementSplitPos(0.05)) \
	KEYBIND(TerminalClient, ({ Keys::ctrl('j') }), TAB->selectNextClient()) \
	KEYBIND(TerminalClient, ({ Keys::ctrl('k') }), TAB->selectPrevClient()) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '1' }), TABAREA->setActive(0)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '2' }), TABAREA->setActive(1)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '3' }), TABAREA->setActive(2)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '4' }), TABAREA->setActive(3)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '5' }), TABAREA->setActive(4)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '6' }), TABAREA->setActive(5)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '7' }), TABAREA->setActive(6)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '8' }), TABAREA->setActive(7)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '9' }), TABAREA->setActive(8)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, '0' }), TABAREA->setActive(9)) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, Keys::Tab }), TABAREA->setLastActive()) \
	KEYBIND(TerminalClient, ({ Keys::CtrlSpace, 'c' }), TAB->nextLayout()) \
	KEYBIND(TerminalClient, ({ Keys::ctrl('w') }), CLIENT->enterWindowMoveMode())
};

const KeyBinds windowMoveKeyBinds = {
	GLOBAL_KEY_BINDS(TerminalClient)
	WINDOW_MOVE_KEY_BINDS(TerminalClient)
	KEYBIND(TerminalClient, ({ Keys::Escape }), CLIENT->exitWindowMoveMode())
	KEYBIND(TerminalClient, ({ Keys::ctrl('c') }), CLIENT->exitWindowMoveMode())
	KEYBIND(TerminalClient, ({ Keys::ctrl('w') }), CLIENT->exitWindowMoveMode())
};

}; // anon namespace


TerminalClient::TerminalClient(Tab* tab)
: Client(tab), mVt(&mBuffer) {
	setSelectable(true);
	mKeyBinds = &keyBinds;
	mAliases = &aliases;
}

TerminalClient::TerminalClient(Tab* tab, std::string command)
: Client(tab), mVt(&mBuffer, command) {
	setSelectable(true);
	mKeyBinds = &keyBinds;
	mAliases = &aliases;
}

void TerminalClient::enterWindowMoveMode(void) {
	mKeyBinds = &windowMoveKeyBinds;
}

void TerminalClient::exitWindowMoveMode(void) {
	mKeyBinds = &keyBinds;
}

WString TerminalClient::getTitle(void) {
	return mVt.getTitle();
}

Vec2 TerminalClient::getCursor(void) {
	if (mVt.isCursorHidden() == false) {
		return getRealPos(mVt.getCursorPos());
	} else {
		return getRealPos(Vec2(0, 0));
	}
}

void TerminalClient::onResize(void) {
	mVt.onResize();
}

void TerminalClient::onKeyDown(Key key) {
	interpret(key);
}

void TerminalClient::unhandledKey(Key key) {
	mVt.send(key);
}

void TerminalClient::onPrerender(void) {
	mVt.step();
	if (mVt.isOpen() == false) {
		close();
	}
}
