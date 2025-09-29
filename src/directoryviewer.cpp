#include <vidd/directoryviewer.hpp>

#include <vidd/filesystem.hpp>
#include <vidd/log.hpp>
#include <vidd/vidd.hpp>

DirectoryViewer::DirectoryViewer(const std::string& path, std::set<std::string>* sel) {
	mSelected = sel;
	onChange = [](){};
	loadDirectory(path);
}

void DirectoryViewer::fileChange(void) {
	onChange();
}

void DirectoryViewer::adjustView(void) {
	std::size_t half = mSize.y / 2;
	if (mPtr < mView || mPtr >= mView + mSize.y) {
		mView = mPtr - half;
	}
}

void DirectoryViewer::loadDirectory(const std::string& path) {
	mPath = FileSystem::realPath(path);
	std::vector<std::string> filePaths = FileSystem::getDirectoryContents(mPath);
	std::sort(filePaths.begin(), filePaths.end());

	mFiles.clear();
	for (const auto& path : filePaths) {
		FileInfo f;
		f.path = path;
		f.hasPermission = FileSystem::hasPermission(path);
		f.type = FileSystem::getFileType(path);
		mFiles.push_back(f);
	}

	mPtr = 0;
	mView = 0;
	fileChange();
	requireRedraw();
}

void DirectoryViewer::firstFile(void) {
	mPtr = 0;
	mView = 0;
	fileChange();
	requireRedraw();
}

void DirectoryViewer::lastFile(void) {
	mPtr = mFiles.size() - 1;
	if ((int)mPtr <= mSize.y / 2) {
		mView = 0;
	} else {
		mView = mPtr - mSize.y / 2;
	}
	fileChange();
	requireRedraw();
}

void DirectoryViewer::nextFile(void) {
	if (mPtr == mFiles.size() - 1) return;
	mPtr += 1;
	if (mPtr - mView == (std::size_t)mSize.y) {
		mView += 1;
	}
	fileChange();
	requireRedraw();
}

void DirectoryViewer::prevFile(void) {
	if (mPtr == 0) return;
	mPtr -= 1;
	if (mPtr < mView) {
		mView -= 1;
	}
	fileChange();
	requireRedraw();
}

void DirectoryViewer::setPtr(std::size_t ptr) {
	mPtr = ptr;
	adjustView();
	requireRedraw();
}

void DirectoryViewer::setPtr(std::string name) {
	for (std::size_t i = 0; i < mFiles.size(); i++) {
		if (FileSystem::getFileName(mFiles[i].path) == name) {
			mPtr = i;
			adjustView();
			requireRedraw();
			return;
		}
	}
}

void DirectoryViewer::moveView(int y) {
	if ((int)mView + y < 0) {
		mView = 0;
	} else if (mView + y >= mFiles.size()) {
		mView = mFiles.size() - 1;
	} else {
		mView += y;
	}

	if (mPtr < mView) {
		mPtr = mView;
		fileChange();
	}

	if (mPtr >= mView + mSize.y) {
		mPtr = mView + mSize.y - 1;
		fileChange();
	}

	requireRedraw();
}

void DirectoryViewer::render(void) {
	const Theme* theme = Vidd::getTheme();
	Draw::style(theme->text);
	drawFilledBox(Vec2::zero(), mSize, ' ');

	int y = 0;
	for (auto it = mFiles.begin() + mView; it != mFiles.end() && y < mSize.y; it++) {
		Style style;

		switch (it->type) {
		case FileType::Directory: style = theme->getSyntaxStyle(StyleType::Number); break;
		case FileType::Binary: style = theme->getSyntaxStyle(StyleType::KeyWord); break;
		case FileType::Text: style = theme->text; break;
		case FileType::Special: style = theme->getSyntaxStyle(StyleType::String); break;
		}

		if (y + mView == mPtr) {
			style += Style(Style::reverse);
		}
		if (mSelected != nullptr) {
			if (mSelected->count(it->path) > 0) {
				style += Style(theme->highlight);
			}
		}
		Draw::style(style);

		drawText(Vec2(0, y), WString(FileSystem::getFileName(it->path)));
		y++;
	}
}
