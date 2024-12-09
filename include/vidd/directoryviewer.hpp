#ifndef __VIDD_DIRECTORY_VIEWER_HPP__
#define __VIDD_DIRECTORY_VIEWER_HPP__

#include <vidd/client.hpp>
#include <vidd/filesystem.hpp>

#include <vector>
#include <functional>
#include <string>

struct FileInfo {
	FileType type;
	std::string path;
	bool hasPermission;
};

class DirectoryViewer : public Component {
	std::string mPath;
	std::vector<FileInfo> mFiles;
	std::size_t mView = 0;
	std::size_t mPtr = 0;

public:
	std::function<void(void)> onChange;

public:
	DirectoryViewer(const std::string& path);

	const std::vector<FileInfo>& getFiles(void) { return mFiles; };
	const FileInfo& getSelectedFile(void) {
		return mFiles[mPtr];
	};

	void setPtr(std::size_t ptr);
	std::size_t getPtr(void) { return mPtr; };
	void setPtr(std::string name);

	void fileChange(void);

	void adjustView(void);

	void loadDirectory(const std::string& path);

	void firstFile(void);
	void lastFile(void);
	void nextFile(void);
	void prevFile(void);

	void moveView(int y);

	void render(void) override;
};

#endif
