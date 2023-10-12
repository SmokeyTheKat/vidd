#include <vidd/line.hpp>

Line* Line::createChain(void) {
	Line* id = new Line();
	id->insertLine();
	return id;
}

void Line::deleteChain(Line* line) {
	line = line->getId();
	while (line) {
		Line* next = line->mNext;
		delete line;
		line = next;
	}
}

Line* Line::remove(void) {
	if (isId()) return this;
	if (isIsolated()) {
		this->data.clear();
		return this;
	} else {
		Line* nextLine = nullptr;
		mPrev->mNext = mNext;
		nextLine = mPrev;
		if (!isTail()) {
			mNext->mPrev = mPrev;
			nextLine = mNext;
			nextLine->adjustNumbersAfter();
		}
		delete this;
		return nextLine;
	}
}

bool Line::isId(void) {
	return mPrev == nullptr;
}

bool Line::isHead(void) {
	return mPrev != nullptr && mPrev->mPrev == nullptr;
}

bool Line::isTail(void) {
	return mNext == nullptr;
}

bool Line::isIsolated(void) {
	return isHead() && isTail();
}

Line* Line::skip(int count) {
	Line* line = this;
	if (count == 0) {
		return line;
	} else if (count > 0) {
		while (!line->isTail() && count-- > 0) line = line->mNext;
		return line;
	} else {
		count *= -1;
		while (!line->isHead() && count-- > 0) line = line->mPrev;
		return line;
	}
}

Line* Line::get(int number) {
	Line* line = this;

	if (number > line->number) {
		while (!line->isTail() && number != line->number) {
			line = line->mNext;
		}
	} else if (number < line->number) {
		while (!line->isHead() && number != line->number) {
			line = line->mPrev;
		}
	}

	return line;
}

Line* Line::next(void) {
	return mNext;
}

Line* Line::prev(void) {
	if (mPrev != nullptr && mPrev->isId()) return nullptr;
	return mPrev;
}

Line* Line::getId(void) {
	Line* line = this;
	while (!line->isId()) line = line->mPrev;
	return line;
}

Line* Line::first(void) {
	if (isId()) return mNext;
	Line* line = this;
	while (!line->isHead()) line = line->mPrev;
	return line;
}

Line* Line::last(void) {
	Line* line = this;
	while (!line->isTail()) line = line->mNext;
	return line;
}

void Line::adjustNumbersAfter(void) {
	if (isId()) return;
	Line* line = this;
	while (line) {
		if (!line->mPrev->isId()) {
			line->number = line->mPrev->number + 1;
		} else {
			line->number = 0;
		}
		line = line->mNext;
	}
}

Line* Line::insertLine(void) {
	Line* newLine = new Line();
	if (!isTail()) {
		newLine->mNext = mNext;
		mNext->mPrev = newLine;
	}
	mNext = newLine;
	newLine->mPrev = this;

	newLine->number = this->number + 1;
	newLine->adjustNumbersAfter();

	return newLine;
}

Line* Line::insertLineUp(void) {
	if (isId()) return insertLine();
	return mPrev->insertLine();
}

void Line::splitAt(int pos) {
	Line* newLine = insertLine();
	newLine->data.insert(
		newLine->data.begin(),
		this->data.begin() + pos,
		this->data.end()
	);
	this->data.removeAfter(pos);
}
