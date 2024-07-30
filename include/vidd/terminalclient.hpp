#ifndef __VIDD_TERMINALCLIENT_HPP__
#define __VIDD_TERMINALCLIENT_HPP__

#include <vidd/client.hpp>
#include <vidd/virtualterminal.hpp>

class TerminalClient : public Client {
	VirtualTerminal mVt;
	bool mStartedWindowModeFromMouse = false;
	Vec2 mWindowDragLatch;
	Vec2 mWindowDragOrigSize;

public:
	TerminalClient(Tab* tab);
	TerminalClient(Tab* tab, std::string command);

	void enterWindowMoveMode(void);
	void exitWindowMoveMode(void);

	WString getTitle(void) override;
	void onMouseButtonUp(Vec2 pos) override;
	void onRightMouseButtonDown(Vec2 pos) override;
	void onRightMouseButtonDrag(Vec2 pos) override;
	void onLeftMouseButtonDown(Vec2 pos) override;
	void onLeftMouseButtonDrag(Vec2 pos) override;
	void onLeftMouseButtonDoubleDown(Vec2 pos) override;
	Vec2 getCursor(void) override;
	void onResize(void) override;
	void onKeyDown(Key key) override;
	void unhandledKey(Key key) override;
	void onPrerender(void) override;
};

#endif
