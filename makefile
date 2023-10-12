PREFIX := ~/.local

CC := clang
LIBS :=
CFLAGS := $(if $(uselibfmt), -DUSE_LIBFMT,) -std=c++20 -O3 -Wall -Wextra -Wno-unused-parameter -I./include/ -DPREFIX=\"$(shell realpath $(PREFIX))\"
LDFLAGS := -lstdc++ -lm $(if $(uselibfmt), -lfmt,)

TARGET := ./vidd

CSRCS=$(shell find ./src/ -name "*.cpp" -type f)
OBJS=$(CSRCS:.cpp=.o)

all: $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(PREFIX)/bin/

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	find ./ -type f -name *.o -delete
	rm -rf $(TARGET)

tc: all
	$(TARGET)
