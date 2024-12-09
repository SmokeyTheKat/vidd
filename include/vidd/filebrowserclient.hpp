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
	std::string mCopyedPath;
	std::string mOrigionalDirectory;

public:
	FileBrowserClient(Tab* tab, const std::string& path);
	FileBrowserClient(Tab* tab, const std::string& path, const std::string& defaultFile);

	DirectoryViewer* getDv(void) { return &mDv; };
	std::string getOrigionalDirectory(void) { return mOrigionalDirectory; };

	void adjustView(void);
	void loadDirectory(const std::string& path);
	void reloadDirectory(void);

	void gotoLetter(void);

	void selectFile(void);
	void selectFileNewWindow(void);

	void backDirectory(void);

	void copyFile(void);
	void pasteFile(void);
	void pasteFileAs(void);
	void duplicateFile(void);
	void deleteFile(void);
	void renameFile(void);
	void makeFile(void);
	void makeDir(void);

	void setCwd(void);

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
