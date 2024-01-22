#include <vidd/filesystem.hpp>

#include <vidd/wchar.hpp>

#include <filesystem>
#include <cstdio>
#include <vector>

#include <unistd.h>

namespace {

bool isFileBinary(std::string path) {
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

	for (std::size_t i = 0; length >= 4 && i < length - 4;) {
		if (buffer[i] == 0) return true;
		if (!WChar::utf8valid((char*)&buffer[i])) {
			return true;
		}
		i += WChar::utf8len((char*)&buffer[i]);
	}

	return false;
}

} // anon namespace

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

std::string FileSystem::getContainingDirectory(std::string path) {
	if (path.back() == '/') {
		path.pop_back();
	}

	auto lastSlash = path.rfind('/');
	if (lastSlash == std::string::npos) lastSlash = 0;
	else lastSlash += 1;

	return path.substr(0, lastSlash);
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

bool FileSystem::isTextFile(std::string path) {
	return FileSystem::getFileType(path) == FileType::Text;
}

bool FileSystem::isBinaryFile(std::string path) {
	return FileSystem::getFileType(path) == FileType::Binary;
}

bool FileSystem::hasPermission(std::string path) {
	return ::access(path.c_str(), R_OK) == 0;
}

FileType FileSystem::getFileType(std::string path) {
	std::filesystem::file_type type = std::filesystem::status(path).type();
	switch (type) {
	case std::filesystem::file_type::directory: {
		return FileType::Directory;
	} break;
	case std::filesystem::file_type::regular: {
		if (FileSystem::hasPermission(path) && isFileBinary(path)) {
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

void FileSystem::copy(std::string from, std::string to) {
	std::filesystem::copy(
		from,
		to,
		std::filesystem::copy_options::overwrite_existing
		| std::filesystem::copy_options::recursive
	);
}

void FileSystem::remove(std::string path) {
	std::filesystem::remove_all(path);
}

void FileSystem::rename(std::string path, std::string name) {
	std::filesystem::rename(path, getContainingDirectory(path) + name);
}

void FileSystem::createFile(std::string name) {
	std::FILE* fp = std::fopen(name.c_str(), "w");
	std::fclose(fp);
}

void FileSystem::createDirectory(std::string name) {
	std::filesystem::create_directory(name);
}

void FileSystem::setCwd(std::string path) {
	std::filesystem::current_path(path);
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

std::vector<std::string> FileSystem::getAllSubTextFiles(std::string path) {
	std::vector<std::string> paths;
	for (const auto& p : std::filesystem::recursive_directory_iterator(std::filesystem::path(path))) {
		std::string file = p.path().string();
		if (FileSystem::isTextFile(file)) {
			paths.push_back(p.path().string());
		}
	}
	return paths;
}
