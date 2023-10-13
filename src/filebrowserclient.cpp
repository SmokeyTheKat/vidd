#include <vidd/filebrowserclient.hpp>

#include <vidd/terminalclient.hpp>
#include <vidd/texteditorclient.hpp>
#include <vidd/virtualterminal.hpp>
#include <vidd/filesystem.hpp>
#include <vidd/vidd.hpp>
#include <vidd/log.hpp>
#include <vidd/keybinds.hpp>
#include <vidd/codeview.hpp>
#include <vidd/texteditor.hpp>
#include <vidd/input.hpp>
#include <vidd/format.hpp>

#include <algorithm>

#define DV CLIENT->getDv()

const KeyBinds normalKeyBinds = {
	GLOBAL_KEY_BINDS(FileBrowserClient)
	WINDOW_KEY_BINDS(FileBrowserClient)
	KEYBIND(FileBrowserClient, ({ Keys::ctrl('c') }), CLIENT->close())
	KEYBIND(FileBrowserClient, ({ 'g', 'g' }), DV->firstFile())
	KEYBIND(FileBrowserClient, ({ 'g', 'h' }), CLIENT->loadDirectory("./"))
	KEYBIND(FileBrowserClient, ({ 'g', 'm', 'm' }), CLIENT->loadDirectory("/mnt"))
	KEYBIND(FileBrowserClient, ({ 'g', 'm', 'p' }), CLIENT->loadDirectory("/home/william/Documents/Projects"))
	KEYBIND(FileBrowserClient, ({ 'G' }), DV->lastFile())
	KEYBIND(FileBrowserClient, ({ 'j' }), DV->nextFile())
	KEYBIND(FileBrowserClient, ({ 'k' }), DV->prevFile())
	KEYBIND(FileBrowserClient, ({ 'J' }), DV->moveView(1))
	KEYBIND(FileBrowserClient, ({ 'K' }), DV->moveView(-1))
	KEYBIND(FileBrowserClient, ({ 'l' }), CLIENT->selectFile())
	KEYBIND(FileBrowserClient, ({ Keys::Return }), CLIENT->selectFile())
	KEYBIND(FileBrowserClient, ({ Keys::CtrlBackslash }), CLIENT->selectFileNewWindow())
	KEYBIND(FileBrowserClient, ({ 'h' }), CLIENT->backDirectory())
	KEYBIND(FileBrowserClient, ({ 'f' }), CLIENT->gotoLetter())
	KEYBIND(FileBrowserClient, ({ Keys::ctrl('w') }), CLIENT->enterWindowMoveMode())
};

const KeyBinds windowMoveKeyBinds = {
	GLOBAL_KEY_BINDS(FileBrowserClient)
	WINDOW_MOVE_KEY_BINDS(FileBrowserClient)
	KEYBIND(FileBrowserClient, ({ Keys::Escape }), CLIENT->exitWindowMoveMode())
	KEYBIND(FileBrowserClient, ({ Keys::ctrl('c') }), CLIENT->exitWindowMoveMode())
	KEYBIND(FileBrowserClient, ({ Keys::ctrl('w') }), CLIENT->exitWindowMoveMode())
};

const AliasBinds aliases = {
	WINDOW_ALIASES
};

class CodeViewer : public Component {
	TextEditor mEditor;
	CodeView mCodeView;

public:
	CodeViewer(Input input)
	: mEditor(std::move(input.limitLines(300))), mCodeView(&mEditor) {
	};

	void onAttach(void) override {
		addChild(&mCodeView);
	};

	void onResize(void) override {
		mEditor.resize(mSize);
		mCodeView.setSize(mSize);
	};
};

class TerminalViewer : public Component {
	VirtualTerminal mVt;

public:
	TerminalViewer(const std::string& cmd)
	: mVt(&mBuffer, cmd) {
	};

	void onResize(void) override {
		mVt.onResize();
	};

	void onPrerender(void) override {
		mVt.step();
	};
};

FileBrowserClient::FileBrowserClient(Tab* tab, const std::string& path)
: Client(tab), mPath(FileSystem::realPath(path)), mDv(path), mParentDv(FileSystem::getParentDirectory(path)) {
	mDv.onChange = std::bind(&FileBrowserClient::fileChange, this);
	setSelectable(true);
	mKeyBinds = &normalKeyBinds;
	mAliases = &aliases;

	loadDirectory(mPath);
}

FileBrowserClient::FileBrowserClient(Tab* tab, const std::string& path, const std::string& defaultFile)
: FileBrowserClient(tab, path) {
	mDv.setPtr(FileSystem::getFileName(defaultFile));
	fileChange();
}

void FileBrowserClient::gotoLetter(void) {
	Key letter = Terminal::getKey();
	if (CharSets::characters.contains(letter)) {
		const auto& files = mDv.getFiles();
		for (std::size_t i = 0; i < files.size(); i++) {
			std::string name = FileSystem::getFileName(files[i].path);
			if (name[0] >= (char)letter) {
				mDv.setPtr(i);
				return;
			}
		}
	}
}

void FileBrowserClient::loadDirectory(const std::string& path) {
	mPath = path;
	mDv.loadDirectory(mPath);
	mParentDv.loadDirectory(FileSystem::getParentDirectory(mPath));
	mParentDv.setPtr(FileSystem::getFileName(mPath));
}

void FileBrowserClient::selectFile(void) {
	const FileInfo& file = mDv.getSelectedFile();
	if (file.hasPermission == false) return;
	if (file.type == FileType::Directory) {
		loadDirectory(file.path);
	} else if (file.type == FileType::Text) {
		getTab()->replaceClientWithNew<TextEditorClient>(this, Input(file.path));
	}
}

void FileBrowserClient::selectFileNewWindow(void) {
	const FileInfo& file = mDv.getSelectedFile();
	if (file.hasPermission == false) return;
	if (file.type == FileType::Text) {
		getTab()->addClient<TextEditorClient>(Input(file.path));
	}
}

void FileBrowserClient::backDirectory(void) {
	loadDirectory(FileSystem::getParentDirectory(mPath));
}

void FileBrowserClient::fileChange(void) {
	mSide.reset(nullptr);
	const FileInfo& file = mDv.getSelectedFile();
	if (file.type == FileType::Directory) {
		if (file.hasPermission) {
			mSide.reset(new DirectoryViewer(file.path));
		}
	} else if (file.type == FileType::Binary) {
		if (
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".png") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".gif") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".jpg")
		) {
			mSide.reset(new TerminalViewer(Format::format("termimg {} -a -d -c -k", file.path)));
		} else if (
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".mp3") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".wav")
		) {
			mSide.reset(new TerminalViewer(Format::format("mpv {}", file.path)));
		}
	} else if (file.type == FileType::Text) {
		mSide.reset(new CodeViewer(Input(file.path)));
	}
	if (mSide != nullptr) {
		onResize();
		addChild(mSide.get());
	}
}

void FileBrowserClient::enterWindowMoveMode(void) {
	mKeyBinds = &windowMoveKeyBinds;
}

void FileBrowserClient::exitWindowMoveMode(void) {
	mKeyBinds = &normalKeyBinds;
}

WString FileBrowserClient::getTitle(void) {
	return mPath;
}

void FileBrowserClient::onResize(void) {
	mParentDv.setSize(Vec2(mSize.x * 0.1, mSize.y));
	mDv.setPos(Vec2(mSize.x * 0.1 + 1, 0));
	mDv.setSize(Vec2(mSize.x * 0.20 - 2, mSize.y));
	if (mSide != nullptr) {
		mSide->setPos(Vec2(mSize.x * 0.30, 0));
		mSide->setSize(Vec2(mSize.x * 0.70, mSize.y));
	}
}

void FileBrowserClient::onAttach(void) {
	addChild(&mParentDv);
	addChild(&mDv);
}

void FileBrowserClient::onKeyDown(Key key) {
	interpret(key);
}

void FileBrowserClient::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(theme->text);
	drawFilledBox(Vec2::zero(), mSize, ' ');
}
