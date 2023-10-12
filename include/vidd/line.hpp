#ifndef __VIDD_LINE_HPP__
#define __VIDD_LINE_HPP__

#include <vidd/wchar.hpp>

#include <string>
#include <iostream>

struct Line {
	Line* next = nullptr;
	Line* prev = nullptr;
	int number = 0;
	WString data;

	static Line* createChain(void);
	static void deleteChain(Line* line);

	Line(const Line&) = delete;
	Line(Line&&) = delete;

	Line& operator=(const Line&) = delete;
	Line& operator=(Line&&) = delete;

	Line* remove(void);

	bool isHead(void);
	bool isIsolated(void);

	Line* first(void);
	Line* last(void);
	Line* skip(int count);
	Line* get(int number);

	void adjustNumbersAfter(void);
	Line* insertLine(void);
	Line* insertLineUp(void);

	void splitAt(int pos);

private:
	Line(void) = default;
	~Line(void) = default;
};

#endif
