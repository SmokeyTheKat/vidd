#include <vidd/terminal.hpp>

#include <vidd/style.hpp>
#include <vidd/keys.hpp>
#include <vidd/utils.hpp>
#include <vidd/log.hpp>
#include <vidd/format.hpp>

#include <cstdio>
#include <string>
#include <stack>
#include <algorithm>
#include <memory>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

static struct termios oldt;
static int sClickDepth = 0;
static Vec2 sLastClick = Vec2(-1, -1);
static Vec2 sLastPos = Vec2(0, 0);
static Vec2 sLastLastClick = Vec2(-1, -1);
static Terminal::CursorStyle sCurrentCursor = Terminal::CursorStyle::Normal;

static std::unique_ptr<FrameBuffer> sBuffer;
static std::unique_ptr<FrameBuffer> sBackBuffer;
static std::string sTextBuffer;
static Vec2 sSize;

static std::stack<Event> sStagedEvents;

Key Terminal::getRawKey(void) {
	unsigned char chr;
	while (::read(STDIN_FILENO, &chr, 1) == 0);
	return chr;
}

void Terminal::stageEvent(Event e) {
	sStagedEvents.push(e);
}

std::size_t Terminal::stagedEventsLeft(void) {
	return sStagedEvents.size();
}

Key Terminal::getKey(void) {
	Event e;
	while ((e = Terminal::getEvent()).type != EventType::Key);
	return std::get<KeyEvent>(e.event).key;
}

Event Terminal::getEvent(void) {
	Event e = getUnprocessedEvent();
	switch (e.type) {
	case EventType::Key: {
		KeyEvent ke = std::get<KeyEvent>(e.event);
		switch (ke.key) {
		case Keys::ScrollUp:
		case Keys::ScrollDown:
		case Keys::ScrollLeft:
		case Keys::ScrollRight: {
			switch (sClickDepth) {
			case 1: Terminal::stageEvent(Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonDrag, sLastPos))); break;
			case 2: Terminal::stageEvent(Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonDoubleDrag, sLastPos))); break;
			case 3: Terminal::stageEvent(Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonTripleDrag, sLastPos))); break;
			default: break;
			}
			return e;
		} break;
		default: break;
		}
	} break;
	case EventType::Mouse: {
		MouseEvent me = std::get<MouseEvent>(e.event);
		sLastPos = me.pos;
		if (me.type == MouseEventType::LeftButtonDrag) {
			switch (sClickDepth) {
			case 1: return Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonDrag, me.pos));
			case 2: return Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonDoubleDrag, me.pos));
			case 3: return Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonTripleDrag, me.pos));
			default: break;
			}
		} else if (me.type == MouseEventType::LeftButtonDown) {
			if (me.pos == sLastClick) {
				if (me.pos == sLastLastClick) {
					sClickDepth = 3;
					return Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonTripleDown, me.pos));
				} else {
					sClickDepth = 2;
					sLastLastClick = me.pos;
					return Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonDoubleDown, me.pos));
				}
			} else {
				sClickDepth = 1;
				sLastClick = me.pos;
			}
		} else if (me.type == MouseEventType::ButtonUp) {
			sClickDepth = 0;
		} else if (me.type != MouseEventType::ButtonUp) {
			sLastClick = Vec2(-1, -1);
			sLastLastClick = Vec2(-1, -1);
		}
	} break;
	default: break;
	}
	return e;
}

Event Terminal::getUnprocessedEvent(void) {
	if (sStagedEvents.size() > 0) {
		Event e = sStagedEvents.top();
		sStagedEvents.pop();
		return e;
	}

	::fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	::timeval delay;
	delay.tv_usec = 30000;
	delay.tv_sec = 0;
	::select(1, &fds, NULL, NULL, &delay);

	unsigned char chr;
	if (::read(STDIN_FILENO, &chr, 1) <= 0) {
		return Event(EventType::None, NoneEvent());
	}

	while (1) {
		switch (chr) {
		case Keys::Escape: {
			unsigned char next[10];
			if (::read(STDIN_FILENO, next, 1) == 0) {
				for (int i = 0; i < 20; i++) ::read(STDIN_FILENO, next, 1);
				return Event(EventType::Key, KeyEvent(Keys::Escape));
			}
			if (::read(STDIN_FILENO, next+1, 1) == 0) return Event(EventType::Key, KeyEvent(Keys::Escape));

			if (next[0] == '[') {
				if (next[1] >= '0' && next[1] <= '9') {
					if (::read(STDIN_FILENO, next+2, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));

					if (next[1] == '1' && next[2] == ';') {
						if (::read(STDIN_FILENO, next+3, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (::read(STDIN_FILENO, next+4, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (next[3] != '5') Event(EventType::Key, KeyEvent(Keys::Escape));
						switch (next[4]) {
						case 'A': return Event(EventType::Key, KeyEvent(Keys::CtrlUp));
						case 'B': return Event(EventType::Key, KeyEvent(Keys::CtrlDown));
						case 'C': return Event(EventType::Key, KeyEvent(Keys::CtrlRight));
						case 'D': return Event(EventType::Key, KeyEvent(Keys::CtrlLeft));
						case 'S': return Event(EventType::ForceQuit, NoneEvent());;
						}
					} else if (next[1] == '2' && next[2] == '0') {
						if (::read(STDIN_FILENO, next+3, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (next[3] == '~') {
							return Event(EventType::Key, KeyEvent(Keys::F9));
						}
						if (::read(STDIN_FILENO, next+4, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (next[4] != '~') Event(EventType::Key, KeyEvent(Keys::Escape));
						switch (next[3]) {
						case '0': return Event(EventType::Paste, PasteEvent(PasteEventType::Start));
						case '1': return Event(EventType::Paste, PasteEvent(PasteEventType::End));
						}
					} else if (next[1] == '3' && next[2] == ';') {
						if (::read(STDIN_FILENO, next+3, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (::read(STDIN_FILENO, next+4, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (next[3] != ';') Event(EventType::Key, KeyEvent(Keys::Escape));
						if (next[4] != '~') Event(EventType::Key, KeyEvent(Keys::Escape));
						return Event(EventType::Key, KeyEvent(Keys::CtrlDelete));
					} else if (next[2] == '~') {
						switch (next[1]) {
						case '3': return Event(EventType::Key, KeyEvent(Keys::Delete));
						case '5': return Event(EventType::Key, KeyEvent(Keys::PageUp));
						case '6': return Event(EventType::Key, KeyEvent(Keys::PageDown));
						}
					} else if (next[1] == '1') {
						if (::read(STDIN_FILENO, next+3, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (next[3] != '~') Event(EventType::Key, KeyEvent(Keys::Escape));
						switch (next[2]) {
						case '5': return Event(EventType::Key, KeyEvent(Keys::F5));
						case '7': return Event(EventType::Key, KeyEvent(Keys::F6));
						case '8': return Event(EventType::Key, KeyEvent(Keys::F7));
						case '9': return Event(EventType::Key, KeyEvent(Keys::F8));
						}
					} else if (next[1] == '2') {
						if (::read(STDIN_FILENO, next+3, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (next[3] != '~') Event(EventType::Key, KeyEvent(Keys::Escape));
						switch (next[2]) {
						case '1': return Event(EventType::Key, KeyEvent(Keys::F10));
						case '3': return Event(EventType::Key, KeyEvent(Keys::F11));
						case '4': return Event(EventType::Key, KeyEvent(Keys::F12));
						}
					}
				} else if (next[1] == 'M') {
					if (::read(STDIN_FILENO, next+2, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));

					if (next[2] == '!') {
						if (::read(STDIN_FILENO, next+3, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
						if (::read(STDIN_FILENO, next+4, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
							if (next[3] == 'C' && next[4] == '9') {
								Event(EventType::Key, KeyEvent(Keys::MiddleMouseDown));
							}
					} else {
						unsigned char x, y;
						::read(STDIN_FILENO, &x, 1);
						::read(STDIN_FILENO, &y, 1);
						Vec2 pos(x - 33, y - 33);

						switch (next[2]) {
						case 'C': return Event(EventType::Mouse, MouseEvent(MouseEventType::Move, pos));
						case '@': return Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonDrag, pos));
						case ' ': return Event(EventType::Mouse, MouseEvent(MouseEventType::LeftButtonDown, pos));
						case '"': return Event(EventType::Mouse, MouseEvent(MouseEventType::RightButtonDown, pos));
						case 'B': return Event(EventType::Mouse, MouseEvent(MouseEventType::RightButtonDrag, pos));
						case '#': return Event(EventType::Mouse, MouseEvent(MouseEventType::ButtonUp, pos));
						case 'a': return Event(EventType::Key, KeyEvent(Keys::ScrollDown));
						case '`': return Event(EventType::Key, KeyEvent(Keys::ScrollUp));
						case 'b': return Event(EventType::Key, KeyEvent(Keys::ScrollLeft));
						case 'c': return Event(EventType::Key, KeyEvent(Keys::ScrollRight));
						}
					}
				} else {
					switch (next[1]) {
					case 'A': return Event(EventType::Key, KeyEvent(Keys::Up));
					case 'B': return Event(EventType::Key, KeyEvent(Keys::Down));
					case 'C': return Event(EventType::Key, KeyEvent(Keys::Right));
					case 'D': return Event(EventType::Key, KeyEvent(Keys::Left));
					case 'H': return Event(EventType::Key, KeyEvent(Keys::Home));
					case 'F': return Event(EventType::Key, KeyEvent(Keys::End));
					case 'Z': return Event(EventType::Key, KeyEvent(Keys::ShiftTab));
					}
				}
			} else if (next[0] == 'O') {
				if (::read(STDIN_FILENO, next+1, 1) == 0) Event(EventType::Key, KeyEvent(Keys::Escape));
				switch (next[1]) {
				case 'P': return Event(EventType::Key, KeyEvent(Keys::F1));
				case 'Q': return Event(EventType::Key, KeyEvent(Keys::F2));
				case 'R': return Event(EventType::Key, KeyEvent(Keys::F3));
				case 'S': return Event(EventType::Key, KeyEvent(Keys::F4));
				case 'H': return Event(EventType::Key, KeyEvent(Keys::Home));
				case 'F': return Event(EventType::Key, KeyEvent(Keys::End));
				}
			}
		} break;
		default: {
			if (chr <= 127) {
				return Event(EventType::Key, KeyEvent(chr));
			} else {
				char buf[16] = { static_cast<char>(chr), 0 };
				int buf_len = 1;
				while (true) {
					std::mblen(0, 0);
					int chrlen = std::mblen(buf, buf_len);
					if (chrlen < 0) {
						if (::read(STDIN_FILENO, &buf[buf_len++], 1) == 0) {
							return Event(EventType::Key, KeyEvent(0));
						}
						continue;
					} else {
						buf[buf_len] = 0;
						return Event(EventType::Key, KeyEvent(WChar(buf, chrlen).value));
					}
				}
				return Event(EventType::Key, KeyEvent(0));
			}
		} break;
		}
	}
}

bool Terminal::flush(void) {
	sTextBuffer.clear();

	Terminal::hideCursor(sTextBuffer);
	Terminal::saveCursor(sTextBuffer);
	Terminal::Cursor::moveTo(Vec2(0, 0), sTextBuffer);

	std::size_t initialLength = sTextBuffer.length();

	auto mainPixels = sBuffer->collasped();
	auto backPixels = sBackBuffer->collasped();

	int width = sBuffer->getSize().x;

	int x = 0;
	int xSkipped = 0;
	int ySkipped = 0;
	Style lastStyle;
	bool atEnd = false;
	for (
		auto mainIt = mainPixels.begin(), backIt = backPixels.begin();
		mainIt != mainPixels.end();
		mainIt++, backIt++
	) {
		const auto& px = *mainIt;
		const auto& bpx = *backIt;
		if (px == bpx) {
			x += 1;
			if (x == width) {
				ySkipped += 1;
				xSkipped = 0;
				x = 0;
			} else {
				xSkipped += 1;
			}
			continue;
		} else {
			if (atEnd) {
				ySkipped += 1;
				atEnd = false;
			}
			if (ySkipped > 0) {
				sTextBuffer += Format::format("\e[{}E", ySkipped);
				ySkipped = 0;
			}
			if (xSkipped > 0) {
				sTextBuffer += Format::format("\e[{}C", xSkipped);
				xSkipped = 0;
			}
		}

		if (px.style != lastStyle) {
			lastStyle.differenceString(sTextBuffer, px.style);
			lastStyle = px.style;
		}

		sTextBuffer += px.character.view();
		x += 1;
		atEnd = x == width;
		if (atEnd) x = 0;
	}

	if (sTextBuffer.length() != initialLength) {
//        Log::log(std::to_string(sTextBuffer.length() - initialLength));
		Terminal::restoreCursor(sTextBuffer);
		Terminal::showCursor(sTextBuffer);
		std::fwrite(sTextBuffer.data(), 1, sTextBuffer.length(), stdout);
		std::fflush(stdout);
		sBackBuffer->copy(*sBuffer);
		return true;
	} else {
		return false;
	}
}

FrameBuffer* Terminal::getFrameBuffer(void) {
	return sBuffer.get();
}

WString Terminal::getPasteData(void) {
	WString data;
	while (1) {
		Event e = Terminal::getEvent();
		if (e.type == EventType::Paste) {
			PasteEvent pe = std::get<PasteEvent>(e.event);
			if (pe.type == PasteEventType::End) {
				return data;
			}
		} else if (e.type == EventType::Key) {
			KeyEvent ke = std::get<KeyEvent>(e.event);
			if (ke.key == '\r') ke.key = '\n';
			data.pushBack(ke.key);
		}
	}
}

void Terminal::testGetKey(void) {
	std::printf("'q' to exit:\r\n");
	while (1) {
		Key key = Terminal::getRawKey();
		if (key == 'q') break;
		std::printf("%d\r\n", key);
	}
}

void Terminal::clear(void) {
	std::printf("\e[2J" NOSTYLE);
}

void Terminal::setSize(Vec2 size) {
	sBuffer->resize(size);
	sBackBuffer->resize(size);
	sBackBuffer->clear();
}

void Terminal::suspend(void) {
	Terminal::reset();
	Terminal::loadScreen();
	::raise(SIGTSTP);
	Terminal::init();
	Terminal::saveScreen();
	Terminal::setTitle("vidd");
}

Vec2 Terminal::getSize(void) {
	Vec2 size;

	struct winsize w;
	::ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	size.x = w.ws_col;
	size.y = w.ws_row;

	return size;
}

void Terminal::setTitle(std::string title) {
	std::printf("\e]0;%s\x07", title.c_str());
}

void Terminal::saveScreen(void) {
	std::printf("\033[?1049h");
}

void Terminal::loadScreen(void) {
	std::printf("\e[?1049l");
}

void Terminal::setCursor(Terminal::CursorStyle cursor) {
	if (cursor != sCurrentCursor) {
		std::printf("\e[%d q", (int)cursor);
		sCurrentCursor = cursor;
	}
}

void Terminal::showCursor(void) {
	std::printf("\e[?25h");
}

void Terminal::showCursor(std::string& output) {
	output.append("\e[?25h");
}

void Terminal::hideCursor(void) {
	std::printf("\e[?25l");
}

void Terminal::hideCursor(std::string& output) {
	output.append("\e[?25l");
}

void Terminal::saveCursor(void) {
	std::printf("\e[s");
}

void Terminal::saveCursor(std::string& output) {
	output.append("\e[s");
}

void Terminal::restoreCursor(void) {
	std::printf("\e[u");
}

void Terminal::restoreCursor(std::string& output) {
	output.append("\e[u");
}

void Terminal::reset(void) {
	::tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
	Terminal::disableMouse();
	Terminal::disableBracketedPaste();
	Terminal::setCursor(Terminal::CursorStyle::Normal);
}

void Terminal::init(void) {
	std::cin.clear();
	std::freopen("/dev/tty", "rw", stdin);

	::tcgetattr(STDIN_FILENO, &oldt);
	struct termios newt = oldt;
	newt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	newt.c_oflag &= ~(OPOST);
	newt.c_cflag |= (CS8);
	newt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	newt.c_cc[VMIN] = 0;
	newt.c_cc[VTIME] = 0;
	::tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);

	sSize = getSize();

	sTextBuffer.reserve(sSize.x * sSize.y * 4);

	sBuffer = std::make_unique<FrameBuffer>(sSize);
	sBackBuffer = std::make_unique<FrameBuffer>(sSize);

	Terminal::enableMouse();
	Terminal::enableBracketedPaste();
}

void Terminal::enableMouse(void) {
	std::printf("\e[?1003h");
}

void Terminal::disableMouse(void) {
	std::printf("\e[?1003l");
}

void Terminal::enableBracketedPaste(void) {
	std::printf("\e[?2004h");
}

void Terminal::disableBracketedPaste(void) {
	std::printf("\e[?2004l");
}

void Terminal::Cursor::moveTo(Vec2 pos, std::string& output) {
	output.append(Format::format("\e[{};{}H", pos.y+1, pos.x+1));
}

void Terminal::Cursor::moveDown(int v, std::string& output) {
	if (v == 0) return;
	if (v > 0) {
		output.append(Format::format("\e[{}B", v));
	} else {
		output.append(Format::format("\e[{}A", -v));
	}
}

void Terminal::Cursor::moveUp(int v, std::string& output) {
	Terminal::Cursor::moveDown(-v, output);
}

void Terminal::Cursor::moveRight(int v, std::string& output) {
	if (v == 0) return;
	if (v > 0) {
		output.append(Format::format("\e[{}C", v));
	} else {
		output.append(Format::format("\e[{}D", -v));
	}
}

void Terminal::Cursor::moveLeft(int v, std::string& output) {
	Terminal::Cursor::moveRight(-v, output);
}

void Terminal::Cursor::move(Vec2 d, std::string& output) {
	Terminal::Cursor::moveRight(d.x, output);
	Terminal::Cursor::moveDown(d.y, output);
}

void Terminal::Cursor::moveTo(Vec2 pos) {
	std::string data;
	Terminal::Cursor::moveTo(pos, data);
	std::cout << data;
}

void Terminal::Cursor::moveDown(int v) {
	std::string data;
	Terminal::Cursor::moveDown(v, data);
	std::cout << data;
}

void Terminal::Cursor::moveUp(int v) {
	std::string data;
	Terminal::Cursor::moveUp(v, data);
	std::cout << data;
}

void Terminal::Cursor::moveRight(int v) {
	std::string data;
	Terminal::Cursor::moveRight(v, data);
	std::cout << data;
}

void Terminal::Cursor::moveLeft(int v) {
	std::string data;
	Terminal::Cursor::moveLeft(v, data);
	std::cout << data;
}

void Terminal::Cursor::move(Vec2 d) {
	std::string data;
	Terminal::Cursor::move(d, data);
	std::cout << data;
}
