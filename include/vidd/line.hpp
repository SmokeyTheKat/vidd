#ifndef __VIDD_LINE_HPP__
#define __VIDD_LINE_HPP__

#include <vidd/wchar.hpp>

struct Line {
private:
	Line* mNext = nullptr;
	Line* mPrev = nullptr;

public:
	int number = 0;
	WString data;

	static Line* createChain(void);
	static void deleteChain(Line* line);

	Line(const Line&) = delete;
	Line(Line&&) = delete;

	Line& operator=(const Line&) = delete;
	Line& operator=(Line&&) = delete;

	Line* remove(void);

	bool isId(void);
	bool isHead(void);
	bool isTail(void);
	bool isIsolated(void);

	Line* next(void);
	Line* prev(void);

	Line* getId(void);
	Line* first(void);
	Line* last(void);
	Line* skip(int count);
	Line* get(int number);

	void adjustNumbersAfter(void);
	Line* insertLine(void);
	Line* insertLineUp(void);

	void splitAt(int pos);

	bool isEmpty(void);

	int getFirstChar(void);

private:
	Line(void) = default;
	~Line(void) = default;
};

#endif
