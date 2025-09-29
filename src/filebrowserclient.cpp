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
#include <vidd/inputbox.hpp>

#include <algorithm>

#define DV CLIENT->getDv()

const KeyBinds normalKeyBinds = {
	GLOBAL_KEY_BINDS(FileBrowserClient)
	WINDOW_KEY_BINDS(FileBrowserClient)
	KEYBIND(FileBrowserClient, ({ Keys::ctrl('c') }), CLIENT->close())
	KEYBIND(FileBrowserClient, ({ 'g', 'g' }), DV->firstFile())
	KEYBIND(FileBrowserClient, ({ 'g', 'h' }), CLIENT->loadDirectory("./"))
	KEYBIND(FileBrowserClient, ({ 'g', 'o' }), CLIENT->loadDirectory(CLIENT->getOrigionalDirectory()))
	KEYBIND(FileBrowserClient, ({ 'g', 'm', 'm' }), CLIENT->loadDirectory("/mnt"))
	KEYBIND(FileBrowserClient, ({ 'g', 'm', 'p' }), CLIENT->loadDirectory("/home/william/Documents/Projects"))
	KEYBIND(FileBrowserClient, ({ 'y', 'y' }), CLIENT->copyFile())
	KEYBIND(FileBrowserClient, ({ 'y', 'c' }), CLIENT->cutFile())
	KEYBIND(FileBrowserClient, ({ 'p' }), CLIENT->pasteFile())
	KEYBIND(FileBrowserClient, ({ 'P' }), CLIENT->pasteFileAs())
	KEYBIND(FileBrowserClient, ({ 'd', 'd' }), CLIENT->deleteFile())
	KEYBIND(FileBrowserClient, ({ 'r' }), CLIENT->renameFile())
	KEYBIND(FileBrowserClient, ({ 'c', 'c' }), CLIENT->duplicateFile())
	KEYBIND(FileBrowserClient, ({ 'c', 'd' }), CLIENT->makeDir())
	KEYBIND(FileBrowserClient, ({ 'c', 'f' }), CLIENT->makeFile())
	KEYBIND(FileBrowserClient, ({ 's' }), CLIENT->setCwd())
	KEYBIND(FileBrowserClient, ({ 'G' }), DV->lastFile())
	KEYBIND(FileBrowserClient, ({ 'j' }), DV->nextFile())
	KEYBIND(FileBrowserClient, ({ 'k' }), DV->prevFile())
	KEYBIND(FileBrowserClient, ({ 'v' }), CLIENT->select())
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
: Client(tab),
  mPath(FileSystem::realPath(path)),
  mDv(path, &mSelected),
  mParentDv(FileSystem::getParentDirectory(path)),
  mOrigionalDirectory(path) {
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
	if (mDv.getFiles().size() == 0) return;
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
	mPath = FileSystem::realPath(path);
	mSelected.clear();
	mDv.loadDirectory(mPath);
	mParentDv.loadDirectory(FileSystem::getParentDirectory(mPath));
	mParentDv.setPtr(FileSystem::getFileName(mPath));
}

void FileBrowserClient::reloadDirectory(void) {
	if (mDv.getFiles().size() == 0) {
		mDv.loadDirectory(mPath);
	} else {
		std::string file = mDv.getSelectedFile().path;
		mDv.loadDirectory(mPath);
		mDv.setPtr(FileSystem::getFileName(file));
	}
}


void FileBrowserClient::select(void) {
	if (mDv.getFiles().size() == 0) return;
	const FileInfo& file = mDv.getSelectedFile();
	if (mSelected.count(file.path) == 0) {
		mSelected.insert(file.path);
	} else {
		mSelected.erase(file.path);
	}
}

void FileBrowserClient::selectFile(void) {
	if (mDv.getFiles().size() == 0) return;
	const FileInfo& file = mDv.getSelectedFile();
	if (file.hasPermission == false) return;
	if (file.type == FileType::Directory) {
		loadDirectory(file.path);
	} else if (file.type == FileType::Text) {
		getTab()->replaceClientWithNew<TextEditorClient>(this, Input(file.path));
	}
}

void FileBrowserClient::selectFileNewWindow(void) {
	if (mDv.getFiles().size() == 0) return;
	const FileInfo& file = mDv.getSelectedFile();
	if (file.hasPermission == false) return;
	if (file.type == FileType::Text) {
		getTab()->addClient<TextEditorClient>(Input(file.path));
	}
}

void FileBrowserClient::backDirectory(void) {
	loadDirectory(FileSystem::getParentDirectory(mPath));
}

void FileBrowserClient::copyFile(void) {
	if (mDv.getFiles().size() == 0) return;
	mDoCut = false;
	mCopyedPath.clear();
	if (mSelected.size() == 0) {
		const FileInfo& file = mDv.getSelectedFile();
		if (file.hasPermission) {
			mCopyedPath.push_back(file.path);
		}
	} else {
		for (const std::string& path : mSelected) {
			mCopyedPath.push_back(path);
		}
	}
}

void FileBrowserClient::cutFile(void) {
	if (mDv.getFiles().size() == 0) return;
	copyFile();
	mDoCut = true;
}

void FileBrowserClient::pasteFile(void) {
	for (const std::string& path : mCopyedPath) {
		if (mDoCut) FileSystem::move(path, mPath + "/" + FileSystem::getFileName(path));
		else FileSystem::copy(path, mPath + "/" + FileSystem::getFileName(path));
	}
	reloadDirectory();
}

void FileBrowserClient::pasteFileAs(void) {
	if (mCopyedPath.size() == 0) return;
	InputBox* fw = new InputBox("paste with name", [this](InputBox* fw, std::string name) {
		if (name.length() > 0) {
			if (mCopyedPath.size() == 1) {
				if (mDoCut) {
					FileSystem::move(
						mCopyedPath[0],
						FileSystem::getContainingDirectory(mPath) + name
					);
				} else {
					FileSystem::copy(
						mCopyedPath[0],
						FileSystem::getContainingDirectory(mPath) + name
					);
				}
			} else {
				int i = 0;
				for (const std::string& path : mCopyedPath) {
					if (mDoCut) {
						FileSystem::move(
							path,
							FileSystem::getContainingDirectory(mPath) + name + std::to_string(i)
						);
					} else {
						FileSystem::copy(
							path,
							FileSystem::getContainingDirectory(mPath) + name + std::to_string(i)
						);
					}
					i += 1;
				}
			}
			reloadDirectory();
		}
		delete fw;
	});
	getTabArea()->addChild(fw);
	getDisplay()->setSelected(fw);
}

void FileBrowserClient::duplicateFile(void) {
	if (mDv.getFiles().size() == 0) return;
	const FileInfo& file = mDv.getSelectedFile();
	if (!file.hasPermission) return;
	InputBox* fw = new InputBox("duplicate with name", [this, path = file.path](InputBox* fw, std::string name) {
		if (name.length() > 0) {
			FileSystem::copy(
				path,
				FileSystem::getContainingDirectory(path) + name
			);
			reloadDirectory();
		}
		delete fw;
	});
	getTabArea()->addChild(fw);
	getDisplay()->setSelected(fw);
}

void FileBrowserClient::deleteFile(void) {
	const FileInfo& file = mDv.getSelectedFile();
	if (file.hasPermission) {
		InputBox* fw = new InputBox("type \"yes\" to confirm", [this, path = file.path](InputBox* fw, std::string text) {
			if (text == "yes") {
				FileSystem::remove(path);
				reloadDirectory();
			}
			delete fw;
		});
		getTabArea()->addChild(fw);
		getDisplay()->setSelected(fw);
	}
}

void FileBrowserClient::renameFile(void) {
	const FileInfo& file = mDv.getSelectedFile();
	if (file.hasPermission) {
		InputBox* fw = new InputBox("rename", [this, path = file.path](InputBox* fw, std::string name) {
			if (name.length() > 0) {
				FileSystem::rename(path, name);
				loadDirectory(mPath);
			}
			delete fw;
		});
		getTabArea()->addChild(fw);
		getDisplay()->setSelected(fw);
	}
}

void FileBrowserClient::makeFile(void) {
	InputBox* fw = new InputBox("create file", [this](InputBox* fw, std::string name) {
		if (name.length() > 0) {
			FileSystem::createFile(mPath + "/" + name);
			reloadDirectory();
		}
		delete fw;
	});
	getTabArea()->addChild(fw);
	getDisplay()->setSelected(fw);
}

void FileBrowserClient::setCwd(void) {
	FileSystem::setCwd(mPath);
}

void FileBrowserClient::makeDir(void) {
	InputBox* fw = new InputBox("create directory", [this](InputBox* fw, std::string name) {
		if (name.length() > 0) {
			FileSystem::createDirectory(mPath + "/" + name);
			reloadDirectory();
		}
		delete fw;
	});
	getTabArea()->addChild(fw);
	getDisplay()->setSelected(fw);
}

void FileBrowserClient::fileChange(void) {
	mSide.reset(nullptr);
	if (mDv.getFiles().size() == 0) return;
	const FileInfo& file = mDv.getSelectedFile();
	if (file.type == FileType::Directory) {
		if (file.hasPermission) {
			mSide.reset(new DirectoryViewer(file.path));
		}
	} else if (file.type == FileType::Binary) {
		if (
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".png") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".gif") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".jpeg") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".jpg")
		) {
			mSide.reset(new TerminalViewer(Format::format("termimg {} -a -d -c -k", file.path)));
		} else if (
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".mp3") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".ogg") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".ogg") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".caf") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".opus") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".m4a") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".flac") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".wav")
		) {
			mSide.reset(new TerminalViewer(Format::format("mpv --loop '{}'", file.path)));
		} else if (
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".mp4") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".webm") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".mov") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".avi") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".wmv") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".mkv") ||
			FileSystem::hasExtension(Utils::stringToLower(file.path), ".flv")
		) {
			mSide.reset(new TerminalViewer(Format::format("mpv --vo=tct '{}'", file.path)));
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
