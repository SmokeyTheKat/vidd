#ifndef __VIDD_SELECTION_HPP__
#define __VIDD_SELECTION_HPP__

#include <vidd/pos.hpp>
#include <vidd/cursor.hpp>
#include <vidd/line.hpp>
#include <vidd/wchar.hpp>

struct Selection;

enum class SelectionType {
	Normal,
	Word,
	Line,
	Block,
};

enum class SelectionRangeType {
	FirstLine,
	LastLine,
	MiddleLine,
	OnlyLine
};

struct SelectionRange {
	Line* line;
	Vec2 range;
	SelectionRangeType type;

	WStringView getText(void);

	SelectionRange(Line* line, Vec2 range, SelectionRangeType type)
	: line(line), range(range), type(type) {};
};

struct SelectionIterator {
private:
	Line* mLine;
	Line* mLineEnd;
	Line* mNextLine;
	const Selection& mSel;

public:
	SelectionIterator(Line* line, Line* end, const Selection& sel);

	SelectionRange operator*(void);
	SelectionIterator& operator++(void);
	friend bool operator==(SelectionIterator& a, SelectionIterator& b) { return a.mLine == b.mLine; };
	friend bool operator!=(SelectionIterator& a, SelectionIterator& b) { return a.mLine != b.mLine; };
};

struct Selection {
	Cursor curStart;
	Cursor curEnd;
	SelectionType type = SelectionType::Normal;

	Selection ordered(void) const;

	SelectionIterator begin(void) { return SelectionIterator(curStart.y, curEnd.y->next(), *this); }
	SelectionIterator end(void) { return SelectionIterator(curEnd.y->next(), curEnd.y->next(), *this); }
};

#endif
