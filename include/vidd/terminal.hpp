#ifndef __VIDD_TERMINAL_HPP__
#define __VIDD_TERMINAL_HPP__

#include <vidd/pos.hpp>
#include <vidd/keys.hpp>
#include <vidd/event.hpp>
#include <vidd/wchar.hpp>
#include <vidd/framebuffer.hpp>

#include <string>
#include <variant>
#include <iostream>
#include <ostream>

namespace Terminal {
	enum class CursorStyle {
		Normal=0,
		BlinkingBlock=1,
		SteadyBlock=2,
		BlinkingUnderline=3,
		SteadyUnderline=4,
		BlinkingBar=5,
		SteadyBar=6,
	};

	void init(void);
	void reset(void);

	Event getEvent(void);
	Event getUnprocessedEvent(void);
	void stageEvent(Event e);
	std::size_t stagedEventsLeft(void);

	Key getKey(void);
	Key getRawKey(void);
	void testGetKey(void);

	WString getPasteData(void);

	bool flush(void);
	void clear(void);

	FrameBuffer* getFrameBuffer(void);

	Vec2 getSize(void);
	void setSize(Vec2 size);

	void suspend(void);

	void setTitle(std::string title);

	void setCursor(CursorStyle cursor);

	void showCursor(void);
	void showCursor(std::string& output);

	void hideCursor(void);
	void hideCursor(std::string& output);

	void saveCursor(void);
	void saveCursor(std::string& output);

	void restoreCursor(void);
	void restoreCursor(std::string& output);

	void saveScreen(void);
	void loadScreen(void);

	void enableMouse(void);
	void disableMouse(void);

	void enableBracketedPaste(void);
	void disableBracketedPaste(void);

	namespace Cursor {
		void moveTo(Vec2 pos, std::string& output);
		void moveDown(int v, std::string& output);
		void moveUp(int v, std::string& output);
		void moveRight(int v, std::string& output);
		void moveLeft(int v, std::string& output);
		void move(Vec2 d, std::string& output);

		void moveTo(Vec2 pos);
		void moveDown(int v);
		void moveUp(int v);
		void moveRight(int v);
		void moveLeft(int v);
		void move(Vec2 d);
	};
};

#endif
