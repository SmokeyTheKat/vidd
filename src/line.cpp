#include <vidd/line.hpp>

Line* Line::createChain(void) {
	return new Line();
}

void Line::deleteChain(Line* line) {
	line = line->first();
	while (line) {
		Line* next = line->next;
		delete line;
		line = next;
	}
}

Line* Line::remove(void) {
	if (this->prev) {
		Line* nextLine = this->next;
		this->prev->next = nextLine;
		if (nextLine) {
			nextLine->prev = this->prev;
			nextLine->adjustNumbersAfter();
		} else {
			nextLine = this->prev;
		}
		delete this;
		return nextLine;
	} else if (this->next) {
		Line* replace = this->next;
		this->data = std::move(replace->data);
		this->next = replace->next;
		if (replace->next) {
			replace->next->prev = this;
		}
		delete replace;
		this->adjustNumbersAfter();
		return this;
	} else {
		this->data.clear();
		return this;
	}
}

bool Line::isHead(void) {
	return this->prev == nullptr;
}

bool Line::isIsolated(void) {
	return this->prev == nullptr && this->next == nullptr;
}

Line* Line::skip(int count) {
	Line* line = this;
	if (count == 0) {
		return line;
	} else if (count > 0) {
		while (line->next && count-- > 0) line = line->next;
		return line;
	} else {
		count *= -1;
		while (line->prev && count-- > 0) line = line->prev;
		return line;
	}
}

Line* Line::get(int number) {
	Line* line = this;

	if (number > line->number) {
		while (line->next && number != line->number)
			line = line->next;
	} else if (number < line->number) {
		while (line->prev && number != line->number)
			line = line->prev;
	}

	return line;
}

Line* Line::first(void) {
	Line* line = this;
	while (line->prev) line = line->prev;
	return line;
}

Line* Line::last(void) {
	Line* line = this;
	while (line->next) line = line->next;
	return line;
}

void Line::adjustNumbersAfter(void) {
	Line* line = this;
	while (line) {
		if (line->prev) {
			line->number = line->prev->number + 1;
		} else {
			line->number = 0;
		}
		line = line->next;
	}
}

Line* Line::insertLine(void) {
	Line* newLine = new Line();
	if (next) {
		newLine->next = this->next;
		this->next->prev = newLine;
	}
	this->next = newLine;
	newLine->prev = this;

	newLine->number = this->number + 1;
	newLine->adjustNumbersAfter();

	return newLine;
}

Line* Line::insertLineUp(void) {
	if (this->prev) {
		return this->prev->insertLine();
	}

	Line* newLine = this->insertLine();
	newLine->data = std::move(this->data);
	this->data = WString();

	return this;
}

void Line::splitAt(int pos) {
	Line* newLine = this->insertLine();
	newLine->data.insert(
		newLine->data.begin(),
		this->data.begin() + pos,
		this->data.end()
	);
	this->data.removeAfter(pos);
}
