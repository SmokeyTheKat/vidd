#include <vidd/log.hpp>

#include <cstdio>

//#define DEBUG

namespace {

std::vector<std::string> logs;
#ifdef DEBUG
FILE* fp = nullptr;
#endif
}; // namespace

void Log::log(std::string_view msg) {
#ifdef DEBUG
	if (fp == nullptr) {
		fp = std::fopen("./log", "w");
	}
#endif
	std::string str(msg);
	logs.push_back(str);
#ifdef DEBUG
	std::fputs(str.c_str(), fp);
	std::fputc('\n', fp);
	std::fflush(fp);
#endif
}

void Log::clear(void) {
	logs.clear();
}

std::vector<std::string>& Log::get(void) {
	return logs;
}
