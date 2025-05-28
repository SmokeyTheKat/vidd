#ifndef __VIDD_PROCSTREAM_HPP__
#define __VIDD_PROCSTREAM_HPP__

#include <vidd/utils.hpp>
#include <vidd/log.hpp>

#include <iostream>
#include <signal.h>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>

class Process {
	::pid_t mChildPid;
	std::string mCommand;
	bool mIsOpen = false;

	std::string mBuffer;

	int mWriteFd;
	int mReadFd;

public:
	Process(const std::vector<std::string>& args)
	: mCommand(Utils::joinStrings(args, " ")) {
//        std::vector<std::string> sargs = args;
//        std::vector<const char*> cargs;
//        for (std::size_t i = 0; i < sargs.size(); i++) {
//            cargs.push_back(sargs[i].c_str());
//        }
//        cargs.push_back(nullptr);
		int stdinPipe[2];
		int stdoutPipe[2];
		if (::pipe(stdinPipe)) return;
		if (::pipe(stdoutPipe)) return;

		::pid_t pid = ::fork();
		if (pid < 0) return;
		if (pid == 0) {
			::close(stdinPipe[1]);
			::dup2(stdinPipe[0], 0);
			::close(stdoutPipe[0]);
			::dup2(stdoutPipe[1], 1);
//            ::execvp(sargs[0].c_str(), (char**)cargs.data());
			::setpgid(0, 0);
			::execl("/bin/bash", "bash", "-c", mCommand.c_str(), NULL);
			::perror("execl");
			::exit(99);
		}
		mChildPid = pid;
		mWriteFd = stdinPipe[1];
		mReadFd = stdoutPipe[0];
		::close(stdinPipe[0]);
		::close(stdoutPipe[1]);
		mIsOpen = true;
	};

	~Process(void) {
		::killpg(mChildPid, SIGINT);
		::waitpid(mChildPid, NULL, 0);
	};

	int isOpen(void) {
		return ::waitpid(mChildPid, NULL, WNOHANG) == 0;
	};

	bool readReady(void) {
		::fd_set fds;
		FD_ZERO(&fds);
		FD_SET(mReadFd, &fds);
		::timeval delay;
		delay.tv_usec = 0;
		delay.tv_sec = 0;
		return ::select(mReadFd + 1, &fds, NULL, NULL, &delay) > 0;
	};

	std::vector<std::string> readLines(void) {
		std::vector<std::string> out;

		while (readReady()) {
			std::string data = read(128);
			if (data.length() == 0) {
				break;
			}
			mBuffer.insert(mBuffer.end(), data.begin(), data.end());
			std::size_t nl;
			do {
				nl = mBuffer.find_first_of('\n');
				if (nl != std::string::npos) {
					out.push_back(mBuffer.substr(0, nl));
					mBuffer.erase(mBuffer.begin(), mBuffer.begin() + nl + 1);
				}
			} while (nl != std::string::npos);
			break;
		}

		if (!isOpen() && mBuffer.length() > 0) {
			out.push_back(mBuffer);
			mBuffer.clear();
		}
		return out;
	};

	std::string read(std::size_t n) {
		char* buf = new char[n]();
		std::ptrdiff_t length = ::read(mReadFd, buf, n);
		std::string out(buf, length < 0 ? 0 : length);
		delete[] buf;
		return out;
	};

	std::vector<std::string> readAllLines(void) {
		std::vector<std::string> out;
		char buf[512];
		std::string cur;
		while (isOpen()) {
			int length = ::read(mReadFd, buf, sizeof(buf));
			if (length <= 0) break;
			std::string_view chunk(buf, length);
			for (std::size_t i = 0; i < chunk.length(); i++) {
				if (chunk[i] == '\n') {
					out.push_back(std::move(cur));
					cur.clear();
				} else {
					cur += chunk[i];
				}
			}
		}
		out.push_back(cur);
		return out;
	};

	void write(std::string_view data) {
		::write(mWriteFd, data.data(), data.length());
	};

	void endWrite(void) {
		::close(mWriteFd);
	};

	operator bool(void) {
		return mIsOpen;
	};
};

#endif
