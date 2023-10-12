#ifndef __VIDD_VIRTUAL_TERMINAL_HPP__
#define __VIDD_VIRTUAL_TERMINAL_HPP__

#include <vidd/keys.hpp>
#include <vidd/framebuffer.hpp>
#include <vidd/style.hpp>
#include <vidd/parsestring.hpp>

#include <string>

#include <sys/types.h>

class VirtualTerminal {
	::pid_t mPid;
	int mFd;

	Style mBrush = Style(Color(255), Color(0));
	Vec2 mSavedCursor = Vec2(0, 0);
	Vec2 mCursor = Vec2(0, 0);
	bool mCursorHidden = false;
	WString mTitle;
	std::string mCommand;

	FrameBuffer* mFrameBuffer;
	FrameBuffer mSavedBuffer;
	Vec2 mScreenSavedCursor;

	std::string mProcessBuffer;

public:
	VirtualTerminal(FrameBuffer* frameBuffer);
	VirtualTerminal(FrameBuffer* frameBuffer, std::string command);
	~VirtualTerminal(void);

	WString getTitle(void) { return mTitle; };
	bool isCursorHidden(void) { return mCursorHidden; };
	Vec2 getCursorPos(void) { return mCursor; };

	bool isOpen(void);

	void send(Key key);
	void step(void);

	void onResize(void);

private:
	void open(void);

	void newLine(void);

	void shiftRowsUp(void);
	void shiftRowsDown(void);

	void setStyle(int style);

	void interruptOutput(void);
	void readChildStdout(void);
	void interpretCsi(ParseString parse);
};

#endif
