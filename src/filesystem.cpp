#include <vidd/filesystem.hpp>

#include <vidd/wchar.hpp>

#include <filesystem>
#include <cstdio>
#include <vector>

#include <unistd.h>

std::string FileSystem::realPath(std::string path) {
	return std::filesystem::weakly_canonical(std::filesystem::path(path)).string();
}

std::string FileSystem::getFileName(std::string path) {
	return std::filesystem::path(path).filename().string();
}

std::string FileSystem::getParentDirectory(std::string path) {
	if (path.back() != '/') {
		path += '/';
	}
	path += "..";
	return realPath(path);
}

bool FileSystem::hasExtension(std::string path, std::string ext) {
	return ext == path.substr(path.length() - ext.length());
}

bool FileSystem::isDirectory(std::string path) {
	return std::filesystem::is_directory(std::filesystem::path(path));
}

bool FileSystem::isFile(std::string path) {
	return !FileSystem::isDirectory(path);
}

bool FileSystem::isFileBinary(std::string path) {
	std::FILE* fp = std::fopen(path.c_str(), "rb");
	if (fp == nullptr) {
		return false;
	}

	std::uint8_t buffer[128] = {0};
	std::size_t length = fread((char*)buffer, 1, sizeof(buffer), fp);
	std::fclose(fp);
	if (length <= 0) return false;
	if (
		buffer[0] == 0xef &&
		buffer[1] == 0xbb &&
		buffer[0] == 0xef
	) return false;

	if (
		buffer[0] == 0xfe &&
		buffer[1] == 0xff
	) return false;

	for (std::size_t i = 0; i < length - 4;) {
		if (buffer[i] == 0) return true;
		if (!WChar::utf8valid((char*)&buffer[i])) {
			return true;
		}
		i += WChar::utf8len((char*)&buffer[i]);
	}

	return false;
}

bool FileSystem::hasPermission(std::string path) {
//    FILE* fp = std::fopen(path.c_str(), "r+");
//    bool permission = fp != 0;
//    if (fp != 0) std::fclose(fp);
//    return permission;
	return ::access(path.c_str(), R_OK) == 0;
}

FileType FileSystem::getFileType(std::string path) {
	std::filesystem::file_type type = std::filesystem::status(path).type();
	switch (type) {
	case std::filesystem::file_type::directory: {
		return FileType::Directory;
	} break;
	case std::filesystem::file_type::regular: {
		if (FileSystem::hasPermission(path) && FileSystem::isFileBinary(path)) {
			return FileType::Binary;
		} else {
			return FileType::Text;
		}
	} break;
	default: {
		return FileType::Special;
	} break;
	}
}

std::vector<std::string> FileSystem::getDirectoryContents(std::string path) {
	std::vector<std::string> paths;
	for (const auto& p : std::filesystem::directory_iterator(std::filesystem::path(path))) {
		paths.push_back(p.path().string());
	}
	return paths;
}

std::vector<std::string> FileSystem::getAllSubFilesAndDirectories(std::string path) {
	std::vector<std::string> paths;
	for (const auto& p : std::filesystem::recursive_directory_iterator(std::filesystem::path(path))) {
		paths.push_back(p.path().string());
	}
	return paths;
}
