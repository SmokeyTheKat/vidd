#include <vidd/virtualterminal.hpp>

#include <vidd/log.hpp>
#include <vidd/keys.hpp>
#include <vidd/parsestring.hpp>
#include <vidd/format.hpp>
#include <vidd/colortables.hpp>

#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pty.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>

#include <vector>
#include <string>
#include <string_view>
#include <optional>

namespace {

std::optional<int> controlSequenceLength(ParseString parse) {
	if (parse.length() < 3) return {};
	if (parse.pop(2) != "\e[") return {};

	parse.popWhile([](char c) { return c >= 0x30 && c <= 0x3f; });
	if (parse.isEmpty()) return {};

	parse.popWhile([](char c) { return c >= 0x20 && c <= 0x2f; });
	if (parse.isEmpty()) return {};

	if (*parse >= 0x40 && *parse <= 0x7e) {
		parse.pop(1);
		return parse.parsedCount();
	}
	return {};
}

};

VirtualTerminal::VirtualTerminal(FrameBuffer* frameBuffer)
: mTitle("bash"), mCommand("bash"), mFrameBuffer(frameBuffer), mSavedBuffer(frameBuffer->getSize()) {
	open();
}

VirtualTerminal::VirtualTerminal(FrameBuffer* frameBuffer, std::string command)
: mTitle("bash"), mCommand(command), mFrameBuffer(frameBuffer), mSavedBuffer(frameBuffer->getSize()) {
	open();
}

VirtualTerminal::~VirtualTerminal(void) {
	::kill(mPid, SIGKILL);
	::waitpid(mPid, NULL, 0);
}

void VirtualTerminal::onResize(void) {
	mSavedBuffer.resize(mFrameBuffer->getSize());
	struct winsize w = {
		.ws_row = static_cast<unsigned short>(mFrameBuffer->getSize().y - 0),
		.ws_col = static_cast<unsigned short>(mFrameBuffer->getSize().x - 1),
		.ws_xpixel = 0,
		.ws_ypixel = 0
	};
	::ioctl(mFd, TIOCSWINSZ, &w);
}

void VirtualTerminal::open(void) {
	int slave;
	int master;
	::openpty(&master, &slave, NULL, NULL, NULL);
	mPid = ::fork();
	if (mPid == 0) {
		::setsid();
		::dup2(slave, 0);
		::dup2(slave, 1);
		::dup2(slave, 2);
		::ioctl(slave, TIOCSCTTY, 0);
		::close(slave);
		::close(master);
		::execlp("/bin/bash", "/bin/bash", "-c", mCommand.c_str(), NULL);
		::perror("execl");
		::exit(99);
	}
	::close(slave);
	::waitpid(mPid, NULL, WNOHANG);
	mFd = master;
}

void VirtualTerminal::send(Key key) {
	switch (key) {
	case Keys::Up: {
		constexpr std::string_view seq = "\e[A";
		::write(mFd, seq.data(), seq.length());
	} break;
	case Keys::Down: {
		constexpr std::string_view seq = "\e[B";
		::write(mFd, seq.data(), seq.length());
	} break;
	case Keys::Right: {
		constexpr std::string_view seq = "\e[C";
		::write(mFd, seq.data(), seq.length());
	} break;
	case Keys::Left: {
		constexpr std::string_view seq = "\e[D";
		::write(mFd, seq.data(), seq.length());
	} break;
	default: {
		WChar wc(key);
		std::string_view wcView = wc.view();
		::write(mFd, wcView.data(), wcView.length());
	} break;
	}
}

void VirtualTerminal::shiftRowsUp(void) {
	int height = mFrameBuffer->getSize().y;
	for (int i = 0; i < height - 1; i++) {
		auto row1 = mFrameBuffer->getRow(i);
		auto row2 = mFrameBuffer->getRow(i+1);
		std::copy(row2.begin(), row2.end(), row1.begin());
	}
	auto lastRow = mFrameBuffer->getRow(height-1);
	std::fill(lastRow.begin(), lastRow.end(), Pixel());
}

void VirtualTerminal::shiftRowsDown(void) {
	int height = mFrameBuffer->getSize().y;
	for (int i = height - 1; i >= 1; i--) {
		auto row1 = mFrameBuffer->getRow(i);
		auto row2 = mFrameBuffer->getRow(i-1);
		std::copy(row2.begin(), row2.end(), row1.begin());
	}
	auto lastRow = mFrameBuffer->getRow(0);
	std::fill(lastRow.begin(), lastRow.end(), Pixel());
}

void VirtualTerminal::newLine(void) {
	if (mCursor.y + 1 == mFrameBuffer->getSize().y) {
		shiftRowsUp();
	} else {
		mCursor.y++;
	}
	mCursor.x = 0;
}

void VirtualTerminal::setStyle(int style) {
	if (style == 0) {
		mBrush = Style(Color(255), Color(0));
	} else if (style == 3) {
		mBrush += Style::italic;
	} else if (style == 4) {
		mBrush += Style::underline;
	} else if (style == 7) {
		mBrush += Style::reverse;
	} else if (style == 1) {
		mBrush += Style::bold;
	} else if (style >= 30 && style <= 37) {
		mBrush.fg = color16Table[style - 30];
	} else if (style >= 40 && style <= 47) {
		mBrush.bg = color16Table[style - 40];
	} else if (style >= 90 && style <= 97) {
		mBrush.fg = color16Table[8 + style - 90];
	} else if (style >= 100 && style <= 107) {
		mBrush.bg = color16Table[8 + style - 100];
	}
}

void VirtualTerminal::interpretCsi(ParseString seq) {
	seq.pop(2);
	char cap = seq.back();
	if (seq.subString(0, 5) == "38;2;") {
		ParseString parse = seq.subString(5, seq.length() - 1);
		mBrush.fg.r = parse.popUntilAndSkip(';').getInt();
		mBrush.fg.g = parse.popUntilAndSkip(';').getInt();
		mBrush.fg.b = parse.getInt();
	} else if (seq.subString(0, 5) == "48;2;") {
		ParseString parse = seq.subString(5, seq.length() - 1);
		mBrush.bg.r = parse.popUntilAndSkip(';').getInt();
		mBrush.bg.g = parse.popUntilAndSkip(';').getInt();
		mBrush.bg.b = parse.getInt();
	} else if (seq.subString(0, 5) == "38;5;") {
		mBrush.fg = color256Table[seq.subString(5, seq.length() - 1).getInt()];
	} else if (seq.subString(0, 5) == "48;5;") {
		mBrush.bg = color256Table[seq.subString(5, seq.length() - 1).getInt()];
	} else if (seq == "6n") {
		std::string cpr = Format::format("\e[{};{}R", mCursor.y + 1, mCursor.x + 1);
		::write(mFd, cpr.data(), cpr.length());
	} else if (cap == 'A') {
		int count = 1;
		if (seq.length() > 1) {
			count = seq.subString(0, seq.length() - 1).getInt();
		}
		mCursor.y = std::max(0, mCursor.y - count);
	} else if (cap == 'B') {
		int count = 1;
		if (seq.length() > 1) {
			count = seq.subString(0, seq.length() - 1).getInt();
		}
		mCursor.y = std::min(mFrameBuffer->getSize().y - 1, mCursor.y + count);
	} else if (cap == 'C') {
		int count = 1;
		if (seq.length() > 1) {
			count = seq.subString(0, seq.length() - 1).getInt();
		}
		mCursor.x = std::min(mFrameBuffer->getSize().x - 1, mCursor.x + count);
	} else if (cap == 'D') {
		int count = 1;
		if (seq.length() > 1) {
			count = seq.subString(0, seq.length() - 1).getInt();
		}
		mCursor.x = std::max(0, mCursor.x - count);
	} else if (cap == 'E') {
		int count = 1;
		if (seq.length() > 1) {
			count = seq.subString(0, seq.length() - 1).getInt();
		}
		mCursor.y = std::min(mFrameBuffer->getSize().y - 1, mCursor.y + count);
		mCursor.x = 0;
	} else if (cap == 'F') {
		int count = 1;
		if (seq.length() > 1) {
			count = seq.subString(0, seq.length() - 1).getInt();
		}
		mCursor.y = std::max(0, mCursor.y - count);
		mCursor.x = 0;
	} else if (cap == 'G') {
		int col = 1;
		if (seq.length() > 1) {
			col = seq.subString(0, seq.length() - 1).getInt();
		}
		mCursor.x = std::clamp(col, 0, mFrameBuffer->getSize().x);
	} else if (cap == 'J') {
		int mode = 0;
		if (seq.length() > 1) {
			mode = seq.subString(0, seq.length() - 1).getInt();
		}
		switch (mode) {
			case 3:
			case 2: {
				for (auto row : *mFrameBuffer) {
					std::fill(row.begin(), row.end(), Pixel(' ', mBrush));
				}
			};
			default: break;
		}
	} else if (cap == 'K') {
		int mode = 0;
		if (seq.length() > 1) {
			mode = seq.subString(0, seq.length() - 1).getInt();
		}
		switch (mode) {
		case 0: {
			for (int i = mCursor.x; i < (int)mFrameBuffer->getSize().x; i++) {
				(*mFrameBuffer)[mCursor.y][i] = Pixel(' ', mBrush);
			}
		} break;
		case 1: {
			for (int i = 0; i <= mCursor.x; i++) {
				(*mFrameBuffer)[mCursor.y][i] = Pixel(' ', mBrush);
			}
		} break;
		case 2: {
			for (int i = 0; i < (int)mFrameBuffer->getSize().x; i++) {
				(*mFrameBuffer)[mCursor.y][i] = Pixel(' ', mBrush);
			}
		} break;
		}
	} else if (cap == 'S') {
		int lines = 1;
		if (seq.length() > 1) {
			lines = seq.subString(0, seq.length() - 1).getInt();
		}
		Log::log(std::to_string(lines));
		for (int i = 0; i < lines; i++) {
			shiftRowsUp();
		}
	} else if (cap == 'T') {
		int lines = 1;
		if (seq.length() > 1) {
			lines = seq.subString(0, seq.length() - 1).getInt();
		}
		Log::log(std::to_string(lines));
		for (int i = 0; i < lines; i++) {
			shiftRowsDown();
		}
	} else if (seq == "?25h") {
		mCursorHidden = false;
	} else if (seq == "?25l") {
		mCursorHidden = true;
	} else if (seq == "?1049h") {
		mSavedBuffer.merge(*mFrameBuffer, Vec2(0, 0));
		mScreenSavedCursor = mCursor;
	} else if (seq == "?1049l") {
		mFrameBuffer->merge(mSavedBuffer, Vec2(0, 0));
		mCursor = mScreenSavedCursor;
	} else if (cap == 'r') {
	} else if (cap == 's') {
		mSavedCursor = mCursor;
	} else if (cap == 'u') {
		mCursor = mSavedCursor;
	} else if (cap == 'H' || cap == 'f') {
		int x = 1;
		int y = 1;
		if (seq.length() > 1) {
			ParseString parse = seq.subString(0, seq.length() - 1);
			ParseString xSv = parse.popUntilAndSkip(';');
			if (xSv.length() > 0) {
				y = xSv.getInt();
			}
			ParseString ySv = parse;
			if (ySv.length() > 0) {
				x = ySv.getInt();
			}
		}
		Vec2 size = mFrameBuffer->getSize();
		mCursor = Vec2(std::clamp(x - 1, 0, size.x - 1), std::clamp(y - 1, 0, size.y - 1));
	} else if (cap == 'm') {
		ParseString parse = seq.subString(0, seq.length() - 1);
		if (parse.isEmpty()) {
			setStyle(0);
			return;
		}
		while (true) {
			int style = parse.popUntilAndSkip(';').getInt();
			setStyle(style);
			if (parse.isEmpty()) break;
		}
	} else {
		Log::log(seq.view());
	}
}

void VirtualTerminal::interruptOutput(void) {
	while (mProcessBuffer.size() > 0) {
		char chr = mProcessBuffer[0];
		if (chr == '\n') {
			mProcessBuffer.erase(mProcessBuffer.begin());
			newLine();
		} else if (chr == '\r') {
			mProcessBuffer.erase(mProcessBuffer.begin());
			mCursor.x = 0;
		} else if (chr == '\b') {
			mProcessBuffer.erase(mProcessBuffer.begin());
			if (mCursor.x > 0) {
				mCursor.x -= 1;
				(*mFrameBuffer)[mCursor.y][mCursor.x] = Pixel(' ', mBrush);
			}
		} else if (chr == '\t') {
			mProcessBuffer.erase(mProcessBuffer.begin());
			int spaces = 8 - (mCursor.x % 8) + 1;
			if (mCursor.x + spaces >= mFrameBuffer->getSize().x) {
				newLine();
			} else {
				for (int i = 0; i < spaces; i++) {
					(*mFrameBuffer)[mCursor.y][mCursor.x++] = Pixel(' ', mBrush);
				}
			}
		} else if (chr == 127) {
			mProcessBuffer.erase(mProcessBuffer.begin());
			if (mCursor.x > 0) {
				mCursor.x -= 1;
				(*mFrameBuffer)[mCursor.y][mCursor.x] = Pixel(' ', mBrush);
			}
		} else if (mProcessBuffer[0] == '\e') {
			if (mProcessBuffer.length() == 1) return;
			if (mProcessBuffer[1] == '(') {
				if (mProcessBuffer.length() < 3) return;
				mProcessBuffer.erase(mProcessBuffer.begin());
				mProcessBuffer.erase(mProcessBuffer.begin());
				mProcessBuffer.erase(mProcessBuffer.begin());
			} else if (mProcessBuffer[1] == ']') {
				ParseString parse(mProcessBuffer);
				parse.pop(2);
				int mode = parse.popUntilAndSkip(';').getInt();
				std::string str = parse.popUntil('\a').string();
				if (parse.isEmpty() || parse.front() != '\a') return;
				parse.pop(1);
				mProcessBuffer.erase(mProcessBuffer.begin(), mProcessBuffer.begin() + parse.parsedCount());
				switch (mode) {
				case 0: {
					mTitle = str;
				} break;
				}
				(void)mode;
			} else if (mProcessBuffer[1] == '[') {
				auto opt = controlSequenceLength(mProcessBuffer);
				if (opt.has_value() == false) return;
				int length = *opt;
				interpretCsi(ParseString(mProcessBuffer.data(), length));
				mProcessBuffer.erase(mProcessBuffer.begin(), mProcessBuffer.begin() + length);
			} else {
				mProcessBuffer.erase(mProcessBuffer.begin());
			}
		} else if (mProcessBuffer.length() > 0) {
			WChar wc(mProcessBuffer.c_str());
			if (isVisibleWChar(wc) == false) {
				mProcessBuffer.erase(mProcessBuffer.begin());
				return;
				continue;
			}

			for (int i = 0; i < (int)wc.view().length(); i++) {
				mProcessBuffer.erase(mProcessBuffer.begin());
			}

			if (mCursor.x + 1 == mFrameBuffer->getSize().x) newLine();
			(*mFrameBuffer)[mCursor.y][mCursor.x++] = Pixel(wc, mBrush);
		}
	}
}

void VirtualTerminal::readChildStdout(void) {
	char buffer[1024];
	int length = ::read(mFd, buffer, sizeof(buffer));

	if (length <= 0) return;
	std::string_view svBuffer(buffer, length);
	mProcessBuffer.insert(mProcessBuffer.end(), svBuffer.begin(), svBuffer.end());
}

void VirtualTerminal::step(void) {
	::timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 500;

	::fd_set fdSet;
	FD_ZERO(&fdSet);
	FD_SET(mFd, &fdSet);
	while (isOpen()) {
		::select(mFd+1, &fdSet, NULL, NULL, &tv);
		if (FD_ISSET(mFd, &fdSet)) {
			readChildStdout();
			interruptOutput();
		} else {
			break;
		}
	}
}

bool VirtualTerminal::isOpen(void) {
	return ::waitpid(mPid, NULL, WNOHANG) == 0;
}
