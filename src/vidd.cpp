#include <vidd/vidd.hpp>

#include <vidd/client.hpp>
#include <vidd/tab.hpp>
#include <vidd/texteditorclient.hpp>
#include <vidd/filebrowserclient.hpp>
#include <vidd/log.hpp>
#include <vidd/filesystem.hpp>
#include <vidd/input.hpp>

#include <fstream>

Vidd* Vidd::instance = nullptr;

Vidd::Vidd(const Arguments& args)
: mArgs(args) {
	Vidd::instance = this;
	Vidd::setTheme(Themes::getThemeByName("dracula"));
	mDisplay.setRoot(&mTabArea);
};

void Vidd::quit(void) {
	mDisplay.quit();
}

int Vidd::run(void) {
	Tab* tab = mTabArea.getActive();

	if (Vidd::hasFlag("-")) {
		tab->addClient<TextEditorClient>(Input(stdin, "-"));
	}

	Terminal::init();

	if (Vidd::hasFlag("--test-getch")) {
		Terminal::testGetKey();
		quit();
		Terminal::reset();
		return 0;
	}

	Terminal::saveScreen();
	Terminal::setTitle("vidd");
	Terminal::clear();

	for (const auto& path : mArgs.getFiles()) {
		if (FileSystem::isDirectory(path)) {
			tab->addClient<FileBrowserClient>(path);
		} else {
			TextEditorClient* tex = tab->addClient<TextEditorClient>(Input(path));
			if (Vidd::hasFlag("-b")) {
				tex->getEditor()->cursorMoveToLastLine();
			}
	
			if (Vidd::hasValueArg("-l")) {
				tex->getEditor()->cursorMoveToY(std::stoi(Vidd::getValueArg("-l")));
			}
		}
	}

	mDisplay.setSelected(tab->getSelected());

	mDisplay.render();
	mDisplay.mainLoop();

	Terminal::reset();
	Terminal::loadScreen();

	return 0;
}

void Vidd::shuffleDisplay(void) {
	mTabArea.setSize(mTabArea.getSize());
}
