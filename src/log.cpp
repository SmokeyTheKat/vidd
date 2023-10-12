#include <vidd/log.hpp>

#include <cstdio>

namespace {

std::vector<std::string> logs;
//FILE* fp = nullptr;
}; // namespace

void Log::log(std::string_view msg) {
//    if (fp == nullptr) {
//        fp = std::fopen("./log", "w");
//    }
	std::string str(msg);
	logs.push_back(str);
//    std::fputs(str.c_str(), fp);
//    std::fputc('\n', fp);
//    std::fflush(fp);
}

void Log::clear(void) {
	logs.clear();
}

std::vector<std::string>& Log::get(void) {
	return logs;
}
