#include <vidd/selection.hpp>

#include <vidd/wchar.hpp>
#include <vidd/charsets.hpp>

#include <algorithm>

namespace {

int getWordStartAtPos(WStringView data, int pos) {
	if (data.length() == 0) return 0;
	if (pos >= (int)data.length()) pos = data.length() - 1;
	if (CharSets::characters.contains(data[pos])) {
		while (pos > 0 && CharSets::characters.contains(data[pos])) pos--;
		if (pos == 0 && CharSets::characters.contains(data[pos])) return pos;
		return pos + 1;
	} else {
		return pos;
	}
}

int getWordEndAtPos(WStringView data, int pos) {
	if (data.length() == 0) return 0;
	if (pos >= (int)data.length()) pos = data.length() - 1;
	if (CharSets::characters.contains(data[pos])) {
		while (pos + 1 < (int)data.length() && CharSets::characters.contains(data[pos])) pos++;
		if (pos + 1 == (int)data.length() && CharSets::characters.contains(data[pos])) return pos + 1;
		return pos;
	} else {
		return pos + 1;
	}
}

}; // namespace

WStringView SelectionRange::getText(void) {
	return WStringView(line->data.begin() + range.x, line->data.begin() + range.y);
}

SelectionIterator::SelectionIterator(Line* line, Line* end, const Selection& sel)
: mLine(line), mLineEnd(end), mSel(sel) {
	if (line && line != end) {
		mNextLine = line->next();
	} else {
		mNextLine = line;
	}
}

SelectionRange SelectionIterator::operator*(void) { 
	switch (mSel.type) {
	case SelectionType::Word: {
		if (mLine == mSel.curStart.y && mSel.curStart.y == mSel.curEnd.y) {
			return SelectionRange(
				mLine,
				Vec2(
					getWordStartAtPos(mLine->data, mSel.curStart.x),
					getWordEndAtPos(mLine->data, mSel.curEnd.x)
				),
				SelectionRangeType::OnlyLine
			);
		} else if (mLine == mSel.curStart.y) {
			return SelectionRange(
				mLine,
				Vec2(
					getWordStartAtPos(mLine->data, mSel.curStart.x),
					mLine->data.length()
				),
				SelectionRangeType::FirstLine
			);
		} else if (mLine == mSel.curEnd.y) {
			return SelectionRange(
				mLine,
				Vec2(
					0,
					getWordEndAtPos(mLine->data, mSel.curEnd.x)
				),
				SelectionRangeType::LastLine
			);
		} else {
			return SelectionRange(mLine, Vec2(0, mLine->data.length()), SelectionRangeType::MiddleLine);
		}
	} break;
	case SelectionType::Block: {
		return SelectionRange(
			mLine,
			Vec2(
				std::min(mSel.curStart.x, (int)mLine->data.length()),
				std::min(mSel.curEnd.x, (int)mLine->data.length())
			),
			SelectionRangeType::OnlyLine
		);
	} break;
	case SelectionType::Normal: {
		if (mLine == mSel.curStart.y && mSel.curStart.y == mSel.curEnd.y) return SelectionRange(mLine, Vec2(mSel.curStart.x, mSel.curEnd.x), SelectionRangeType::OnlyLine);
		else if (mLine == mSel.curStart.y) return SelectionRange(mLine, Vec2(mSel.curStart.x, mLine->data.length()), SelectionRangeType::FirstLine);
		else if (mLine == mSel.curEnd.y) return SelectionRange(mLine, Vec2(0, mSel.curEnd.x), SelectionRangeType::LastLine);
		else return SelectionRange(mLine, Vec2(0, mLine->data.length()), SelectionRangeType::MiddleLine);
	} break;
	case SelectionType::Line: {
		return SelectionRange(mLine, Vec2(0, mLine->data.length()), SelectionRangeType::MiddleLine);
	} break;
	default: {
		return SelectionRange(mLine, Vec2(0, mLine->data.length()), SelectionRangeType::MiddleLine);
	} break;
	}
}

SelectionIterator& SelectionIterator::operator++(void) {
	mLine = mNextLine;
	if (mLine != nullptr) {
		mNextLine = mLine->next();
	} else {
		mNextLine = nullptr;
	}
	return *this;
}

Selection Selection::ordered(void) const {
	Selection sel = *this;
	if (type == SelectionType::Block) {
		if (curStart.y->number > curEnd.y->number) {
			sel.curStart = curEnd;
			sel.curEnd = curStart;
		}
		if (sel.curStart.x > sel.curEnd.x) {
			std::swap(sel.curStart.x, sel.curEnd.x);
		}
	} else if (curStart.y->number < curEnd.y->number) {
		sel = *this;
	} else if (curStart.y->number > curEnd.y->number) {
		sel.curStart = curEnd;
		sel.curEnd = curStart;
	} else if (curStart.x < curEnd.x) {
		sel = *this;
	} else if (curStart.x > curEnd.x) {
		sel.curStart = curEnd;
		sel.curEnd = curStart;
	}
	return sel;
};
