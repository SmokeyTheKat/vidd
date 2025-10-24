#include <vidd/colordrawclient.hpp>

#include <vidd/keybinds.hpp>
#include <vidd/texteditorclient.hpp>
#include <vidd/display.hpp>
#include <vidd/tabarea.hpp>
#include <vidd/tab.hpp>
#include <vidd/fuzzywindow.hpp>
#include <vidd/grepwindow.hpp>
#include <vidd/log.hpp>
#include <vidd/vidd.hpp>
#include <vidd/filebrowserclient.hpp>
#include <vidd/timer.hpp>
#include <vidd/utils.hpp>
#include <vidd/theme.hpp>
#include <vidd/filesystem.hpp>
#include <vidd/logclient.hpp>
#include <vidd/format.hpp>
#include <vidd/charsets.hpp>
#include <cstdlib>

#include <fstream>
#include <map>
#include <set>
#include <functional>
#include <iostream>
#include <string>

namespace {

#define EDITOR self->getEditor()

#define COMMAND(com, expect, code) \
	{ com, [](ColorDrawClient * self, const std::vector<std::string_view>& params) { \
			if ((int)params.size() < expect) return; \
			code; \
		} \
	}, \

using CommandList = std::map<std::string_view, std::function<void(ColorDrawClient *, const std::vector<std::string_view>&)>>;

const CommandList commandList = {
	COMMAND("w", 0, CLIENT->save())
	COMMAND("resize", 2, CLIENT->resizeCanvas(std::stoi(std::string(params[0])), std::stoi(std::string(params[1]))))
	COMMAND("color", 0, CLIENT->addColor(params))
};

#define MOVEMENT_KEY_BINDS \
	KEYBIND(ColorDrawClient, ({ 'h' }), CLIENT->cursorMove(Vec2(-1, 0))) \
	KEYBIND(ColorDrawClient, ({ 'l' }), CLIENT->cursorMove(Vec2(1, 0))) \
	KEYBIND(ColorDrawClient, ({ 'j' }), CLIENT->cursorMove(Vec2(0, 1))) \
	KEYBIND(ColorDrawClient, ({ 'k' }), CLIENT->cursorMove(Vec2(0, -1))) \
	KEYBIND(ColorDrawClient, ({ '0' }), CLIENT->cursorMove(Vec2(-10000, 0))) \
	KEYBIND(ColorDrawClient, ({ '$' }), CLIENT->cursorMove(Vec2(10000, 0))) \
	KEYBIND(ColorDrawClient, ({ 'g', '_' }), CLIENT->cursorMoveLastCharacter()) \
	KEYBIND(ColorDrawClient, ({ '^' }), CLIENT->cursorMoveFirstCharacter()) \
	KEYBIND(ColorDrawClient, ({ 'g', 'g' }), CLIENT->cursorMove(Vec2(-100000, -10000))) \
	KEYBIND(ColorDrawClient, ({ 'G' }), CLIENT->cursorMove(Vec2(-100000, 10000))) \
	KEYBIND(ColorDrawClient, ({ Keys::Left }), CLIENT->cursorMove(Vec2(-1, 0))) \
	KEYBIND(ColorDrawClient, ({ Keys::Right }), CLIENT->cursorMove(Vec2(1, 0))) \
	KEYBIND(ColorDrawClient, ({ Keys::Down }), CLIENT->cursorMove(Vec2(0, 1))) \
	KEYBIND(ColorDrawClient, ({ Keys::Up }), CLIENT->cursorMove(Vec2(0, -1))) \

const AliasBinds normalAliases = {
	WINDOW_ALIASES
	{ { 'R', 'f' }, { 'v', 'R', 'f', Keys::Escape } },
	{ { 'R', 'b' }, { 'v', 'R', 'b', Keys::Escape } },
	{ { 'R', 's' }, { 'v', 'R', 's', Keys::Escape } },
	{ { 'R', 'p' }, { 'v', 'R', 'p', Keys::Escape } },
};

const KeyBinds normalKeyBinds = {
	GLOBAL_KEY_BINDS(ColorDrawClient)
	WINDOW_KEY_BINDS(ColorDrawClient)
	MOVEMENT_KEY_BINDS
	KEYBIND(ColorDrawClient, ({ Keys::ctrl('c') }), CLIENT->tryClose())
//    KEYBIND(ColorDrawClient, ({ Keys::ctrl('w') }), CLIENT->enterWindowMoveMode())
	KEYBIND(ColorDrawClient, ({ ':' }), CLIENT->enterCommandMode())
	KEYBIND(ColorDrawClient, ({ '#', '0' }), CLIENT->toggleMergeLayers())
	KEYBIND(ColorDrawClient, ({ ' ', '!' }), CLIENT->returnToTextEditor())
	KEYBIND(ColorDrawClient, ({ '~' }), CLIENT->toggleCaptailizationAtCursor())
	KEYBIND(ColorDrawClient, ({ '#' }), CLIENT->selectLayer())
	KEYBIND(ColorDrawClient, ({ 'f' }), CLIENT->placeChar(); CLIENT->cursorMove(Vec2(1, 0)))
	KEYBIND(ColorDrawClient, ({ 'F' }), CLIENT->placeChar(); CLIENT->cursorMove(Vec2(-1, 0)))
	KEYBIND(ColorDrawClient, ({ 't' }), CLIENT->placeChar(); CLIENT->cursorMove(Vec2(0, 1)))
	KEYBIND(ColorDrawClient, ({ 'T' }), CLIENT->placeChar(); CLIENT->cursorMove(Vec2(0, -1)))
	KEYBIND(ColorDrawClient, ({ 'z' }), CLIENT->selectNewChar())
	KEYBIND(ColorDrawClient, ({ 'Z', 'f' }), CLIENT->selectFg())
	KEYBIND(ColorDrawClient, ({ 'Z', 'b' }), CLIENT->selectBg())
	KEYBIND(ColorDrawClient, ({ 'Z', 's' }), CLIENT->selectFormat())
	KEYBIND(ColorDrawClient, ({ 'Z', 'p' }), CLIENT->selectStyle())
	KEYBIND(ColorDrawClient, ({ 'Z', 'c' }), CLIENT->selectChar())
	KEYBIND(ColorDrawClient, ({ 'i' }), CLIENT->enterInsertMode())
	KEYBIND(ColorDrawClient, ({ 'I' }), CLIENT->enterTextInsertMode())
	KEYBIND(ColorDrawClient, ({ 'a' }), CLIENT->enterInsertMode(); CLIENT->cursorMove(Vec2(1, 0)))
	KEYBIND(ColorDrawClient, ({ 'v' }), CLIENT->enterSelectMode())
	KEYBIND(ColorDrawClient, ({ 'X' }), CLIENT->backspace())
	KEYBIND(ColorDrawClient, ({ 'x' }), CLIENT->doDelete())
	KEYBIND(ColorDrawClient, ({ 'C' }), CLIENT->textBackspace())
	KEYBIND(ColorDrawClient, ({ 'c' }), CLIENT->textDelete())
	KEYBIND(ColorDrawClient, ({ 'r' }), CLIENT->replaceChar())
	KEYBIND(ColorDrawClient, ({ 'p' }), CLIENT->paste())
	KEYBIND(ColorDrawClient, ({ ' ', 'p' }), CLIENT->pasteTransparent())
	KEYBIND(ColorDrawClient, ({ ' ', 'd' }), CLIENT->openDirectory())
};

const AliasBinds insertAliases = {
};

const KeyBinds insertKeyBinds = {
	GLOBAL_KEY_BINDS(ColorDrawClient)
	KEYBIND(ColorDrawClient, ({ Keys::Escape }), CLIENT->exitInsertMode())
	KEYBIND(ColorDrawClient, ({ Keys::Backspace }), CLIENT->backspace())
	KEYBIND(ColorDrawClient, ({ Keys::Left }), CLIENT->cursorMove(Vec2(-1, 0)))
	KEYBIND(ColorDrawClient, ({ Keys::Right }), CLIENT->cursorMove(Vec2(1, 0)))
	KEYBIND(ColorDrawClient, ({ Keys::Down }), CLIENT->cursorMove(Vec2(0, 1)))
	KEYBIND(ColorDrawClient, ({ Keys::Up }), CLIENT->cursorMove(Vec2(0, -1)))
};

const AliasBinds selectAliases = {
};

const KeyBinds selectKeyBinds = {
	GLOBAL_KEY_BINDS(ColorDrawClient)
	MOVEMENT_KEY_BINDS
	KEYBIND(ColorDrawClient, ({ Keys::Escape }), CLIENT->exitSelectMode())
	KEYBIND(ColorDrawClient, ({ 'y' }), CLIENT->copy())
	KEYBIND(ColorDrawClient, ({ 'f' }), CLIENT->fillChar())
	KEYBIND(ColorDrawClient, ({ 'r' }), CLIENT->replaceFillChar())
	KEYBIND(ColorDrawClient, ({ 'F', 'b' }), CLIENT->fillBox(0))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '1' }), CLIENT->fillBox(1))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '2' }), CLIENT->fillBox(2))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '3' }), CLIENT->fillBox(3))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '4' }), CLIENT->fillBox(4))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '5' }), CLIENT->fillBox(5))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '6' }), CLIENT->fillBox(6))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '7' }), CLIENT->fillBox(7))
	KEYBIND(ColorDrawClient, ({ 'F', 'B', '8' }), CLIENT->fillBox(8))
	KEYBIND(ColorDrawClient, ({ 'R', 'f' }), CLIENT->replaceFg())
	KEYBIND(ColorDrawClient, ({ 'R', 'b' }), CLIENT->replaceBg())
	KEYBIND(ColorDrawClient, ({ 'R', 's' }), CLIENT->replaceFormat())
	KEYBIND(ColorDrawClient, ({ 'R', 'p' }), CLIENT->replaceStyle())
	KEYBIND(ColorDrawClient, ({ 'z' }), CLIENT->selectChar())
	KEYBIND(ColorDrawClient, ({ 's' }), CLIENT->swapCursor())
};

const AliasBinds windowMoveAliases = {
	WINDOW_ALIASES
};

const KeyBinds windowMoveKeyBinds = {
	GLOBAL_KEY_BINDS(ColorDrawClient)
//    WINDOW_MOVE_KEY_BINDS(ColorDrawClient)
//    KEYBIND(ColorDrawClient, ({ Keys::Escape }), CLIENT->exitWindowMoveMode())
//    KEYBIND(ColorDrawClient, ({ Keys::ctrl('c') }), CLIENT->exitWindowMoveMode())
//    KEYBIND(ColorDrawClient, ({ Keys::ctrl('w') }), CLIENT->exitWindowMoveMode())
};

const AliasBinds commandAliases = {
};

const KeyBinds commandKeyBinds = {
	GLOBAL_KEY_BINDS(ColorDrawClient)
};

const AliasBinds noAliases = {
};

const KeyBinds noKeyBinds = {
	GLOBAL_KEY_BINDS(ColorDrawClient)
};

}; // namespace

ColorDrawClient::ColorDrawClient(Tab* tab, std::string file)
: Client(tab), mFileName(std::move(file)), mCopyBuffer(Vec2(20, 20))
{
	mFbs.emplace_back(Vec2(40, 20));
	mFb = &mFbs[0];
	setSelectable(true);
	mKeyBinds = &normalKeyBinds;
	mAliases = &normalAliases;
	mStyle = Style(Color(255, 255, 255), Color::none());
	mFb->fill(Pixel(' ', mStyle));
	mMode = Mode::Normal;
	mPalette.push_back(Color::none());
	mPalette.push_back(Color(255, 255, 255));
	mPalette.push_back(Color(0, 0, 0));
	mPalette.push_back(Color(255, 0, 0));
	mPalette.push_back(Color(0, 255, 0));
	mPalette.push_back(Color(0, 0, 255));
	mPalette.push_back(Color(255, 255, 0));
	mPalette.push_back(Color(255, 0, 255));
	mPalette.push_back(Color(0, 255, 255));
	load();
}

std::pair<Vec2, Vec2> ColorDrawClient::getSelectionArea(void) {
	Vec2 pos = mActionStart;
	Vec2 size = mCursor - mActionStart;
	Vec2 sn = Vec2(size.x < 0 ? -1 : 1, size.y < 0 ? -1 : 1);
	size += sn;
	if (size.x < 0) {
		pos.x += size.x + 1;
		size.x *= -1;
	}
	if (size.y < 0) {
		pos.y += size.y + 1;
		size.y *= -1;
	}

	return {pos, size};
}

void ColorDrawClient::save(void) {
	std::string name = mFileName;
	std::ofstream ofs(name);

	Color pfg{255, 255, 255};
	Color pbg{0, 0, 0};
	int y = 0;
	for (int layer = 0; layer < mFbs.size(); layer++) {
		FrameBuffer* fb = &mFbs[layer];
		for (auto line : *fb) {
			for (int x = 0; x < line.length(); x++) {
				Pixel cur = line[x];

				if (pfg != cur.style.fg || pbg != cur.style.bg) {
					ofs << "{";
					if (pfg != cur.style.fg) {
						ofs << "fg[" <<
							std::to_string(cur.style.fg.r) << "," <<
							std::to_string(cur.style.fg.g) << "," <<
							std::to_string(cur.style.fg.b) << "]";
					}
					if (pbg != cur.style.bg) {
						ofs << "bg[" <<
							std::to_string(cur.style.bg.r) << "," <<
							std::to_string(cur.style.bg.g) << "," <<
							std::to_string(cur.style.bg.b) << "]";
					}
					ofs << "}";
				}

				if (cur.character == '{') {
					ofs << "{{";
				} else {
					ofs << cur.character.view();
				}

				pfg = cur.style.fg;
				pbg = cur.style.bg;
			}
			if (y + 1 < fb->getSize().y) ofs << '\n';
			y += 1;
		}
		if (layer + 1 < mFbs.size()) {
			ofs << "{layer}\n";
			y = 0;
		}
	}
	ofs.close();
}

void ColorDrawClient::load(void) {
	std::ifstream ifs(mFileName);
	std::stringstream ss;
	ss << ifs.rdbuf();
	std::string sdata = ss.str();
	std::string_view data = sdata;

	int width = 0;
	for (int i = 0; i < data.length(); i++) {
		if (data[i] == '\n') {
			break;
		} else if (data[i] == '{' && i + 1 < data.length() && data[i + 1] != '{') {
			while (i < data.length() && data[i] != '}') i += 1;
			if (i == data.length()) return;
		} else if (data[i] == '{' && i + 1 < data.length() && data[i + 1] == '{') {
			i += 1;
			width += 1;
		} else {
			width += 1;
		}
	}

	int height = 1;
	int w = 0;
	for (int i = 0; i < data.length(); i++) {
		if (std::string_view(&data[i]).starts_with("{layer}")) {
			if (w != width) return;
			break;
		} else if (data[i] == '\n') {
			if (w != width) return;
			height += 1;
			w = 0;
		} else if (data[i] == '{' && i + 1 < data.length() && data[i + 1] != '{') {
			while (i < data.length() && data[i] != '}') i += 1;
			if (i == data.length()) return;
		} else if (data[i] == '{' && i + 1 < data.length() && data[i + 1] == '{') {
			i += 1;
			w += 1;
		} else {
			w += 1;
		}
	}

	mFb->resize(Vec2(width, height));

	Color fg{255, 255, 255};
	Color bg{0, 0, 0};
	int x = 0;
	int y = 0;
	for (int i = 0; i < data.length(); i++) {
		if (std::string_view(&data[i]).starts_with("{layer}\n")) {
			i += std::string_view("{layer}\n").length();
			fg = Color{255, 255, 255};
			bg = Color{0, 0, 0};
			x = 0;
			y = 0;
			mFbs.emplace_back(mFb->getSize());
			mFb = &mFbs[mFbs.size() - 1];
		}
		char c = data[i];
		char n = i + 1 < data.length() ? data[i + 1] : ' ';
		if (c == '{' && n != '{') {
			i += 1;
			if (data[i] == '}') continue;

			while (data[i] != '}') {
				std::string_view cmd = data.substr(i, 3);
				if (cmd == "fg[" || cmd == "bg[") {
					i += 3;
					std::string_view d = std::string_view(data);
					d.remove_prefix(i);
					std::size_t p = d.find(',');
					if (p == std::string_view::npos) break;
					int r = std::stoi(std::string(d.substr(0, p)));
					d.remove_prefix(p + 1);
					i += p + 1;

					p = d.find(',');
					if (p == std::string_view::npos) break;
					int g = std::stoi(std::string(d.substr(0, p)));
					d.remove_prefix(p + 1);
					i += p + 1;

					p = d.find(']');
					if (p == std::string_view::npos) break;
					int b = std::stoi(std::string(d.substr(0, p)));
					d.remove_prefix(p + 1);
					i += p + 1;

					if (cmd == "fg[") {
						fg.r = r;
						fg.g = g;
						fg.b = b;
					} else if (cmd == "bg[") {
						bg.r = r;
						bg.g = g;
						bg.b = b;
					}
				}
			}
		} else if (c == '\n') {
			y += 1;
			x = 0;
		} else {
			if (c == '{' && n == '{') i += 1;
			(*mFb)[y][x] = Pixel(c, Style(fg, bg));
			x += 1;
		}
	}

	mFb = &mFbs[0];
}

void ColorDrawClient::selectLayer(void) {
	WChar key = Terminal::getKey();
	if (std::isdigit(key)) {
		int id = key - '0' - 1;
		if (id < 0) return;
		if (id < mFbs.size()) {
			mFb = &mFbs[id];
		} else if (id == mFbs.size()) {
			mFbs.emplace_back(mFb->getSize());
			mFb = &mFbs[id];
		}
	}
}


void ColorDrawClient::toggleMergeLayers(void) {
	mMergeLayers = !mMergeLayers;
	requireRedraw();
}

void ColorDrawClient::returnToTextEditor(void) {
	getTab()->replaceClientWithNew<TextEditorClient>(this, mFileName);
}

void ColorDrawClient::copy(void) {
	auto [pos, size] = getSelectionArea();
	mCopyBuffer.resize(size);
	FrameBufferSubArea sa = mFb->subArea(pos, mFb->getSize());
	mCopyBuffer.merge(sa, Vec2(0, 0));
}

void ColorDrawClient::paste(void) {
	mFb->merge(mCopyBuffer, mCursor);
	requireRedraw();
}

void ColorDrawClient::pasteTransparent(void) {
	for (int y = 0; y < mCopyBuffer.getSize().y; y++) {
		for (int x = 0; x < mCopyBuffer.getSize().x; x++) {
			const Pixel& p = mCopyBuffer[y][x];
			if (std::isspace(p.character)) continue;
			int ox = mCursor.x + x;
			int oy = mCursor.y + y;
			if (ox >= mFb->getSize().x) continue;
			if (oy >= mFb->getSize().y) continue;
			(*mFb)[oy][ox] = p;
		}
	}
	requireRedraw();
}

void ColorDrawClient::openDirectory(void) {
	getTab()->replaceClientWithNew<FileBrowserClient>(this, FileSystem::getParentDirectory(mFileName), FileSystem::getFileName(mFileName));
}

void ColorDrawClient::swapCursor(void) {
	std::swap(mCursor, mActionStart);
}

void ColorDrawClient::fillChar(void) {
	auto [pos, size] = getSelectionArea();
	mFb->subArea(pos, size).fill(Pixel(mSelectedChar, mStyle));
	requireRedraw();
}

/*
/````\
|test|
|hi! |
\____/

  ________
 /
|
|
|
 \______
*/

void ColorDrawClient::fillBox(int type) {
	WChar top = mSelectedChar;
	WChar bot = mSelectedChar;
	WChar left = mSelectedChar;
	WChar right = mSelectedChar;
	WChar tl = mSelectedChar;
	WChar tr = mSelectedChar;
	WChar bl = mSelectedChar;
	WChar br = mSelectedChar;
	switch (type) {
	case 1: {
		top = '`';
		bot = '_';
		left = '|';
		right = '|';
		tl = '/';
		tr = '\\';
		bl = '\\';
		br = '/';
	} break;
	case 2: {
		top = '-';
		bot = '-';
		left = '|';
		right = '|';
		tl = '+';
		tr = '+';
		bl = '+';
		br = '+';
	} break;
	case 3: {
		top = '_';
		bot = '_';
		left = '|';
		right = '|';
		tl = ' ';
		tr = ' ';
		bl = '|';
		br = '|';
	} break;
	}

	auto [pos, size] = getSelectionArea();
	for (int x = pos.x; x < pos.x + size.x; x++) {
		(*mFb)[pos.y][x] = Pixel(top, mStyle);
		(*mFb)[pos.y + size.y - 1][x] = Pixel(bot, mStyle);
	}

	for (int y = pos.y; y < pos.y + size.y; y++) {
		(*mFb)[y][pos.x] = Pixel(left, mStyle);
		(*mFb)[y][pos.x + size.x - 1] = Pixel(right, mStyle);
	}

	(*mFb)[pos.y][pos.x] = Pixel(tl, mStyle);
	(*mFb)[pos.y][pos.x + size.x - 1] = Pixel(tr, mStyle);
	(*mFb)[pos.y + size.y - 1][pos.x] = Pixel(bl, mStyle);
	(*mFb)[pos.y + size.y - 1][pos.x + size.x - 1] = Pixel(br, mStyle);
}

void ColorDrawClient::placeChar(void) {
	(*mFb)[mCursor.y][mCursor.x] = Pixel(mSelectedChar, mStyle);
	requireRedraw();
}

void ColorDrawClient::selectNewChar(void) {
	WChar key = Terminal::getKey();
	if (isVisibleWChar(key)) {
		mSelectedChar = key;
	}
}

void ColorDrawClient::selectFg(void) {
	mStyle.fg = (*mFb)[mCursor.y][mCursor.x].style.fg;
}

void ColorDrawClient::selectBg(void) {
	mStyle.bg = (*mFb)[mCursor.y][mCursor.x].style.bg;
}

void ColorDrawClient::selectFormat(void) {
	mStyle.format = (*mFb)[mCursor.y][mCursor.x].style.format;
}

void ColorDrawClient::selectStyle(void) {
	mStyle = (*mFb)[mCursor.y][mCursor.x].style;
}

void ColorDrawClient::selectChar(void) {
	mSelectedChar = (*mFb)[mCursor.y][mCursor.x].character;
}

void ColorDrawClient::replaceFg(void) {
	auto [pos, size] = getSelectionArea();
	for (int y = pos.y; y < pos.y + size.y; y++) {
		for (int x = pos.x; x < pos.x + size.x; x++) {
			(*mFb)[y][x].style.fg = mStyle.fg;
		}
	}
	requireRedraw();
}

void ColorDrawClient::replaceBg(void) {
	auto [pos, size] = getSelectionArea();
	for (int y = pos.y; y < pos.y + size.y; y++) {
		for (int x = pos.x; x < pos.x + size.x; x++) {
			(*mFb)[y][x].style.bg = mStyle.bg;
		}
	}
	requireRedraw();
}

void ColorDrawClient::replaceFormat(void) {
	auto [pos, size] = getSelectionArea();
	for (int y = pos.y; y < pos.y + size.y; y++) {
		for (int x = pos.x; x < pos.x + size.x; x++) {
			(*mFb)[y][x].style.format = mStyle.format;
		}
	}
	requireRedraw();
}

void ColorDrawClient::replaceStyle(void) {
	auto [pos, size] = getSelectionArea();
	for (int y = pos.y; y < pos.y + size.y; y++) {
		for (int x = pos.x; x < pos.x + size.x; x++) {
			(*mFb)[y][x].style = mStyle;
		}
	}
	requireRedraw();
}

void ColorDrawClient::enterCommandMode(void) {
	mMode = Mode::Prompt;
	mKeyBinds = &commandKeyBinds;
	mAliases = &commandAliases;
	mPrompt = TextPrompt(":", {
		.change = [this] {
			requireSelfRedraw();
		},
		.submit = [this] {
			exitCommandMode();

			std::vector<std::string_view> params;

			std::string_view data(mPrompt.get());

			auto comSplit = data.find_first_of(" \r\n\0\t");
			if (comSplit == std::string_view::npos) comSplit = data.length();
			std::string_view cmd = data.substr(0, comSplit);
			if (comSplit < data.length()) {
				std::string_view paramString = data.substr(comSplit + 1);

				while (paramString.length() > 0) {
					auto split = paramString.find_first_of(" \r\n\0\t");
					if (split == std::string_view::npos) split = paramString.length();
					params.push_back(paramString.substr(0, split));
					if (split >= paramString.length()) break;
					paramString = paramString.substr(split + 1);
				}
			}

			if (commandList.find(cmd) != commandList.end()) {
				commandList.at(cmd)(this, params);
			}

			requireRedraw();
		},
		.exit = [this] {
			exitCommandMode();
		}
	});
	requireRedraw();
}

void ColorDrawClient::exitCommandMode(void) {
	mKeyBinds = &normalKeyBinds;
	mAliases = &normalAliases;
	mMode = Mode::Normal;
}

void ColorDrawClient::enterInsertMode(void) {
	mKeyBinds = &insertKeyBinds;
	mAliases = &insertAliases;
	mTextMode = false;
	mMode = Mode::Insert;
	mActionStart = mCursor;
}

void ColorDrawClient::enterTextInsertMode(void) {
	enterInsertMode();
	mTextMode = true;
}

void ColorDrawClient::exitInsertMode(void) {
	mKeyBinds = &normalKeyBinds;
	mAliases = &normalAliases;
	cursorMove(Vec2(-1, 0));
	mMode = Mode::Normal;
}

void ColorDrawClient::enterSelectMode(void) {
	mKeyBinds = &selectKeyBinds;
	mAliases = &selectAliases;
	mMode = Mode::Select;
	mActionStart = mCursor;
}

void ColorDrawClient::exitSelectMode(void) {
	mKeyBinds = &normalKeyBinds;
	mAliases = &normalAliases;
	mMode = Mode::Normal;
}

void ColorDrawClient::resizeCanvas(int width, int height) {
	mFb->resize(Vec2(width, height));
	requireRedraw();
}

void ColorDrawClient::addColor(std::vector<std::string_view> params) {
	Color c;
	if (params.size() == 3) {
		c = Color(std::stoi(std::string(params[0])), std::stoi(std::string(params[1])), std::stoi(std::string(params[2])));
	}

	if (params.size() == 1) {
		if (params[0].length() <= 3) {
			c = Color(std::stoi(std::string(params[0])));
		} else {
			c = Color(params[0]);
		}
	}

	mPalette.push_back(c);
	requireRedraw();
}

void ColorDrawClient::replaceChar(void) {
	WChar key = Terminal::getKey();
	if (isVisibleWChar(key)) {
		(*mFb)[mCursor.y][mCursor.x] = Pixel(key, mStyle);
	}
}

void ColorDrawClient::replaceFillChar(void) {
	WChar key = Terminal::getKey();
	if (isVisibleWChar(key)) {
		WChar old = mSelectedChar;
		mSelectedChar = key;
		fillChar();
		mSelectedChar = old;
	}
}

void ColorDrawClient::backspace(void) {
	cursorMove(Vec2(-1, 0));
	(*mFb)[mCursor.y][mCursor.x].character = ' ';
}

void ColorDrawClient::doDelete(void) {
	(*mFb)[mCursor.y][mCursor.x].character = ' ';
	cursorMove(Vec2(1, 0));
}

void ColorDrawClient::textBackspace(void) {
	cursorMove(Vec2(-1, 0));
	textDelete();
}

void ColorDrawClient::textDelete(void) {
	(*mFb)[mCursor.y][mCursor.x].character = ' ';
	for (int i = mCursor.x; i < mFb->getSize().x - 1; i++) {
		std::swap((*mFb)[mCursor.y][i], (*mFb)[mCursor.y][i + 1]);
	}
}


void ColorDrawClient::cursorMoveFirstCharacter(void) {
	for (int i = 0; i < mFb->getSize().x; i++) {
		if (!std::isspace((*mFb)[mCursor.y][i].character)) {
			mCursor.x = i;
			break;
		}
	}
}

void ColorDrawClient::cursorMoveLastCharacter(void) {
	for (int i = mFb->getSize().x - 1; i >= 0; i--) {
		if (!std::isspace((*mFb)[mCursor.y][i].character)) {
			mCursor.x = i;
			break;
		}
	}
}

void ColorDrawClient::cursorMove(Vec2 d) {
	mCursor += d;
	mCursor.x = std::clamp(mCursor.x, 0, mFb->getSize().x - 1);
	mCursor.y = std::clamp(mCursor.y, 0, mFb->getSize().y - 1);
}

void ColorDrawClient::tryClose(void) {
	close();
}

void ColorDrawClient::toggleCaptailizationAtCursor(void) {
}

void ColorDrawClient::onAttach(void) {
}

Vec2 ColorDrawClient::getCursor(void) {
	if (mMode == Mode::Normal) Terminal::setCursor(Terminal::CursorStyle::SteadyUnderline);
	else if (mMode == Mode::Insert) Terminal::setCursor(Terminal::CursorStyle::SteadyBar);

//    if (mMode == EditMode::Prompt) {
//        return getRealPos(Vec2(mPrompt.getCursor(), mSize.y - 1));
//    } else {
		return getRealPos(mCursor);
//    }
}

void ColorDrawClient::onResize(void) {
}

void ColorDrawClient::onSelect(void) {
	getTab()->setSelected(this);
	requireRedraw();
}

void ColorDrawClient::onDeselect(void) {
	requireRedraw();
}

void ColorDrawClient::onPaste(WStringView data) {
	if (mMode == Mode::Prompt) {
		for (WChar c : data) mPrompt.input(c);
	}
}

void ColorDrawClient::unhandledKey(Key key) {
	if (mMode == Mode::Insert) {
		if (std::isprint(key)) {
			if (mTextMode) {
				for (int i = mFb->getSize().x - 1; i > mCursor.x; i--) {
					std::swap((*mFb)[mCursor.y][i], (*mFb)[mCursor.y][i - 1]);
				}
			}
			(*mFb)[mCursor.y][mCursor.x] = Pixel(key, mStyle);
			requireRedraw();
			cursorMove(Vec2(1, 0));
		} else if (key == Keys::Return) {
			cursorMove(mActionStart - mCursor + Vec2(0, 1));
			mActionStart = mCursor;
		}
	} else if (mMode == Mode::Prompt) {
		mPrompt.input(key);
	}
}

void ColorDrawClient::onKeyDown(Key key) {
	requireSelfRedraw();
	interpret(key);
}

void ColorDrawClient::onLeftMouseButtonDown(Vec2 click) {
	for (int i = 0 ; i < mPalette.size(); i++) {
		Vec2 pos{1 + i * 5, mFb->getSize().y + 1};
		int w = mSize.x - 1;
		pos.y += (pos.x / w) * 3;
		pos.x = pos.x % w;
		if (
			click.x >= pos.x && click.x < pos.x + 4 &&
			click.y >= pos.y && click.y < pos.y + 2
		) {
			mStyle.fg = mPalette[i];
		}
	}
}

void ColorDrawClient::onRightMouseButtonDown(Vec2 click) {
	for (int i = 0 ; i < mPalette.size(); i++) {
		Vec2 pos{1 + i * 5, mFb->getSize().y + 1};
		int w = mSize.x - 1;
		pos.y += (pos.x / w) * 3;
		pos.x = pos.x % w;
		if (
			click.x >= pos.x && click.x < pos.x + 4 &&
			click.y >= pos.y && click.y < pos.y + 2
		) {
			mStyle.bg = mPalette[i];
		}
	}
}

void ColorDrawClient::onPrerender(void) {
}

void ColorDrawClient::renderStatusBar(void) {
	const Theme* theme = Vidd::getTheme();
	if (isSelected()) {
		Draw::style(theme->barActive);
	} else {
		Draw::style(theme->barInactive);
	}

	Vec2 pos = Vec2(0, mSize.y - 1);

	drawFilledBox(pos, Vec2(mSize.x, 1), ' ');

//    if (sIsRecordingMacro) {
//        drawText(pos, WString("@"));
//        pos.x += 1;
//    }

	switch (mMode) {
	case Mode::Normal: {
		drawText(pos, WString("[NORMAL]"));
	} break;
	case Mode::Insert: {
		drawText(pos, WString("[INSERT]"));
	} break;
	case Mode::Select: {
		drawText(pos, WString("[SELECT]"));
	} break;
	case Mode::Prompt: {
		drawText(pos, WString(mPrompt.getLine()));
	} break;
	default: break;
	}

	drawTextReverse(Vec2(mSize.x - 1, pos.y), WString(" "));

	drawTextReverse(WString(Format::format("[{}x{}]", mFb->getSize().x, mFb->getSize().y)));
	drawTextReverse(WString(Format::format("[{},{}]", mCursor.x + 1, mCursor.y + 1)));
	drawTextReverse(WString(Format::format("['{}']", mSelectedChar.string())));
}

void ColorDrawClient::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(theme->text);
	drawFilledBox(Vec2(0, 0), mSize, ' ');
	for (int y = 0; y < mFb->getSize().y; y++) {
		for (int x = 0; x < mFb->getSize().x; x++) {
			Draw::style(Style(Color(0, 0, 0), (x + y) % 2 == 0 ? Color(15) : Color(20)));
			drawFilledBox(Vec2(x, y), Vec2(1, 1), " ");
		}
	}
	if (mMergeLayers) {
		for (FrameBuffer& fb : mFbs) {
			mBuffer.merge2(fb, Vec2(0, 0));
		}
	} else {
		mBuffer.merge2(*mFb, Vec2(0, 0));
	}
	Draw::style(theme->text);
	if (mMode == Mode::Select) {
		auto [pos, size] = getSelectionArea();
		paintFormat(pos, size, theme->highlight);
	}
	drawFilledBox(Vec2(mSize.x - 1, 0), Vec2(1, mSize.y), "│");
	for (int i = 0 ; i < mPalette.size(); i++) {
		Draw::style(Style(Color(0, 0, 0), mPalette[i]));
		Vec2 pos{1 + i * 5, mFb->getSize().y + 1};
		int w = mSize.x - 1;
		pos.y += (pos.x / w) * 3;
		pos.x = pos.x % w;
		drawFilledBox(pos, Vec2(4, 2), " ");
	}
	renderStatusBar();
}

