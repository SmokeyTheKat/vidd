#ifndef __VIDD_FILESYSTEM_HPP__
#define __VIDD_FILESYSTEM_HPP__

#include <string>
#include <vector>

enum class FileType {
	Text,
	Binary,
	Directory,
	Special,
};

namespace FileSystem {
	std::string realPath(std::string path);
	std::string getFileName(std::string path);
	std::string getParentDirectory(std::string path);

	bool hasExtension(std::string path, std::string ext);
	bool isDirectory(std::string path);
	bool isFile(std::string path);
	bool isFileBinary(std::string path);
	bool hasPermission(std::string path);
	FileType getFileType(std::string path);

	std::vector<std::string> getDirectoryContents(std::string path);
	std::vector<std::string> getAllSubFilesAndDirectories(std::string path);
};

#endif
