#ifndef __VIDD_BUFFER_HPP__
#define __VIDD_BUFFER_HPP__

#include <vidd/line.hpp>
#include <vidd/input.hpp>

#include <string>
#include <vector>

struct Buffer {
public:
	Line* head;
	Line* tail;

	Buffer(Input input);
	~Buffer(void);
};

#endif
