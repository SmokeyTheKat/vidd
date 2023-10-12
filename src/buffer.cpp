#include <vidd/buffer.hpp>

#include <vidd/charsets.hpp>
#include <vidd/utils.hpp>

#include <sstream>
#include <algorithm>
#include <iostream>

Buffer::Buffer(Input input) {
	Line* line = Line::createChain();
	head = line;
	std::string data;
	data.reserve(1024);
	std::string lineData;
	lineData.reserve(128);
	while (input.getLine(data)) {
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
		line = line->insertLine();
		if (!head) {
			head = line;
		}
	}
	tail = line;
}

Buffer::~Buffer(void) {
	Line::deleteChain(head);
}
