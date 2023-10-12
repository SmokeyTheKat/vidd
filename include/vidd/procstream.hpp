#ifndef __VIDD_PROCSTREAM_HPP__
#define __VIDD_PROCSTREAM_HPP__

#include <vidd/utils.hpp>
#include <vidd/log.hpp>

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>

class Process {
	::pid_t mChildPid;
	std::string mCommand;
	bool mIsOpen = false;

	int mWriteFd;
	int mReadFd;

public:
	Process(const std::vector<std::string>& args)
	: mCommand(Utils::joinStrings(args, " ")) {
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
		::kill(mChildPid, SIGINT);
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
		return ::select(mReadFd + 1, &fds, NULL, NULL, &delay) == 1;
	};

	std::string read(std::size_t n) {
		char* buf = new char[n]();
		::read(mReadFd, buf, n);
		std::string out(buf);
		delete[] buf;
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
