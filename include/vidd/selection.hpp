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
	Line* mNextLine;
	const Selection& mSel;

public:
	SelectionIterator(Line* line, const Selection& sel)
	: mLine(line), mSel(sel) {
		if (line) {
			mNextLine = line->next();
		} else {
			mNextLine = line;
		}
	}

	SelectionRange operator*(void);
	SelectionIterator& operator++(void);
	friend bool operator==(SelectionIterator& a, SelectionIterator& b) { return a.mLine == b.mLine; };
	friend bool operator!=(SelectionIterator& a, SelectionIterator& b) { return a.mLine != b.mLine; };
};

struct Selection {
	Cursor curStart;
	Cursor curEnd;
	SelectionType type = SelectionType::Normal;

	Selection(void) {};

	Selection ordered(void) const {
		Selection selection = *this;
		if (curStart.y->number < curEnd.y->number) {
			selection = *this;
		} else if (curStart.y->number > curEnd.y->number) {
			selection.curStart = curEnd;
			selection.curEnd = curStart;
		} else if (curStart.x < curEnd.x) {
			selection = *this;
		} else if (curStart.x > curEnd.x) {
			selection.curStart = curEnd;
			selection.curEnd = curStart;
		}
		return selection;
	};

	SelectionIterator begin(void) { return SelectionIterator(curStart.y, *this); }
	SelectionIterator end(void) { return SelectionIterator(curEnd.y->next(), *this); }
};

#endif
