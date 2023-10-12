#ifndef __VIDD_FILE_BROWSER_CLIENT_HPP__
#define __VIDD_FILE_BROWSER_CLIENT_HPP__

#include <vidd/client.hpp>
#include <vidd/directoryviewer.hpp>
#include <vidd/wchar.hpp>

#include <string>
#include <vector>
#include <memory>

class FileBrowserClient : public Client {
	std::string mPath;
	DirectoryViewer mDv;
	DirectoryViewer mParentDv;
	std::unique_ptr<Component> mSide = nullptr;

public:
	FileBrowserClient(Tab* tab, const std::string& path);
	FileBrowserClient(Tab* tab, const std::string& path, const std::string& defaultFile);

	DirectoryViewer* getDv(void) { return &mDv; };

	void adjustView(void);
	void loadDirectory(const std::string& path);

	void gotoLetter(void);

	void selectFile(void);
	void selectFileNewWindow(void);

	void backDirectory(void);

	void fileChange(void);

	void enterWindowMoveMode(void);
	void exitWindowMoveMode(void);

	WString getTitle(void) override;

	void onResize(void) override;
	void onAttach(void) override;
	void onKeyDown(Key key) override;
	void render(void) override;
};

#endif
