#ifndef __VIDD_KEYBINDS_HPP__
#define __VIDD_KEYBINDS_HPP__

#include <vidd/keys.hpp>
#include <vidd/matcher.hpp>

#define TAB self->getTab()
#define TABAREA self->getTabArea()
#define CLIENT self

#define GLOBAL_KEY_BINDS(client) \
	KEYBIND(client, ({ Keys::ctrl('z') }), Terminal::suspend())

#define WINDOW_KEY_BINDS(client) \
	KEYBIND(client, ({ Keys::ctrl('h') }), TAB->incrementSplitPos(-0.05)) \
	KEYBIND(client, ({ Keys::ctrl('l') }), TAB->incrementSplitPos(0.05)) \
	KEYBIND(client, ({ Keys::ctrl('j') }), TAB->selectNextClient()) \
	KEYBIND(client, ({ Keys::ctrl('k') }), TAB->selectPrevClient()) \
	KEYBIND(client, ({ Keys::ctrl('b') }), TABAREA->toggleBar()) \
	KEYBIND(client, ({ Keys::ctrl('t') }), TAB->toggleTabBar()) \
	KEYBIND(client, ({ Keys::ctrl('i') }), TAB->incrementMasterCount(1)) \
	KEYBIND(client, ({ Keys::ctrl('d') }), TAB->incrementMasterCount(-1)) \
	KEYBIND(client, ({ ' ', '1' }), TABAREA->setActive(0)) \
	KEYBIND(client, ({ ' ', '2' }), TABAREA->setActive(1)) \
	KEYBIND(client, ({ ' ', '3' }), TABAREA->setActive(2)) \
	KEYBIND(client, ({ ' ', '4' }), TABAREA->setActive(3)) \
	KEYBIND(client, ({ ' ', '5' }), TABAREA->setActive(4)) \
	KEYBIND(client, ({ ' ', '6' }), TABAREA->setActive(5)) \
	KEYBIND(client, ({ ' ', '7' }), TABAREA->setActive(6)) \
	KEYBIND(client, ({ ' ', '8' }), TABAREA->setActive(7)) \
	KEYBIND(client, ({ ' ', '9' }), TABAREA->setActive(8)) \
	KEYBIND(client, ({ ' ', '0' }), TABAREA->setActive(9)) \
	KEYBIND(client, ({ Keys::F1 }), TABAREA->setActive(0)) \
	KEYBIND(client, ({ Keys::F2 }), TABAREA->setActive(1)) \
	KEYBIND(client, ({ Keys::F3 }), TABAREA->setActive(2)) \
	KEYBIND(client, ({ Keys::F4 }), TABAREA->setActive(3)) \
	KEYBIND(client, ({ Keys::F5 }), TABAREA->setActive(4)) \
	KEYBIND(client, ({ Keys::F6 }), TABAREA->setActive(5)) \
	KEYBIND(client, ({ Keys::F7 }), TABAREA->setActive(6)) \
	KEYBIND(client, ({ Keys::F8 }), TABAREA->setActive(7)) \
	KEYBIND(client, ({ Keys::F9 }), TABAREA->setActive(8)) \
	KEYBIND(client, ({ Keys::F10 }), TABAREA->setActive(9)) \
	KEYBIND(client, ({ ' ', Keys::Tab }), TABAREA->setLastActive()) \
	KEYBIND(client, ({ ' ', 'c' }), TAB->nextLayout()) \
	KEYBIND(client, ({ Keys::CtrlSpace }), CLIENT->toggleFloating()) \

#define WINDOW_ALIASES \
	{ { ' ', ' ' }, { ' ' } }, \

#define WINDOWMOVESPEED 2

#define WINDOW_MOVE_KEY_BINDS(client) \
	KEYBIND(client, ({ 'h' }), CLIENT->windowMoveX(-WINDOWMOVESPEED * 2)) \
	KEYBIND(client, ({ 'j' }), CLIENT->windowMoveY(WINDOWMOVESPEED)) \
	KEYBIND(client, ({ 'k' }), CLIENT->windowMoveY(-WINDOWMOVESPEED)) \
	KEYBIND(client, ({ 'l' }), CLIENT->windowMoveX(WINDOWMOVESPEED * 2)) \
	KEYBIND(client, ({ 'y' }), CLIENT->windowResizeX(-WINDOWMOVESPEED * 2)) \
	KEYBIND(client, ({ 'u' }), CLIENT->windowResizeY(WINDOWMOVESPEED)) \
	KEYBIND(client, ({ 'i' }), CLIENT->windowResizeY(-WINDOWMOVESPEED)) \
	KEYBIND(client, ({ 'o' }), CLIENT->windowResizeX(WINDOWMOVESPEED * 2)) \
	KEYBIND(client, ({ ' ', '1' }), TAB->moveClientToTab(CLIENT, 0); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '2' }), TAB->moveClientToTab(CLIENT, 1); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '3' }), TAB->moveClientToTab(CLIENT, 2); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '4' }), TAB->moveClientToTab(CLIENT, 3); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '5' }), TAB->moveClientToTab(CLIENT, 4); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '6' }), TAB->moveClientToTab(CLIENT, 5); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '7' }), TAB->moveClientToTab(CLIENT, 6); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '8' }), TAB->moveClientToTab(CLIENT, 7); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '9' }), TAB->moveClientToTab(CLIENT, 8); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ ' ', '0' }), TAB->moveClientToTab(CLIENT, 9); CLIENT->exitWindowMoveMode();) \
	KEYBIND(client, ({ Keys::CtrlSpace }), CLIENT->toggleFloating()) \

class Client;

#include <map>
#include <vector>
#include <string_view>
#include <functional>

#define KEYBIND(client, keys, code, ...) \
	{ std::vector<Key>keys, [](void* vself){client* self = reinterpret_cast<client*>(vself); code __VA_ARGS__; (void)self;  } },

#define KEYBIND_PARMS(client, keys, func, ...) \
	{ std::vector<Key>keys, std::bind(&client::func, std::placeholders::_1, __VA_ARGS__) },

#define ALIASBIND(client, akeys, rkeys) \
	{ std::vector<Key>akeys, std::vector<Key>rkeys },

using KeyBinds = Matcher<Key, std::function<void(void*)>>;

using AliasBinds = Matcher<Key, std::vector<Key>>;

#endif
