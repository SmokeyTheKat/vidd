#include <vidd/buffer.hpp>

#include <vidd/charsets.hpp>
#include <vidd/utils.hpp>

#include <algorithm>
#include <iostream>

Buffer::Buffer(Input input) {
	static int a = 0;
	static int b = 0;
	a += 1;
	if (a == 1) {
		b += 1;
	}
	Line* line = Line::createChain()->first();
	head = line;
	std::string data;
	data.reserve(1024);
	std::string lineData;
	lineData.reserve(128);
	bool first = true;
	while (input.getLine(data)) {
		if (!first) {
			line = line->insertLine();
			if (!head) {
				head = line;
			}
		}
		first = false;
		lineData.clear();
		for (char c : data) {
			switch (c) {
			case '\t': {
				lineData.push_back(' ');
				lineData.push_back(' ');
				lineData.push_back(' ');
				lineData.push_back(' ');
			} break;
			case '\r': {
				continue;
			} break;
			default: {
				lineData.push_back(c);
			}
			}
		}
		line->data = lineData;
	}
	tail = line;
}

Buffer::~Buffer(void) {
	Line::deleteChain(head);
}
