PREFIX := ~/.local

CC := clang
LIBS :=
CFLAGS := $(if $(uselibfmt), -DUSE_LIBFMT,) $(if $(default-mode-insert), -DVIDD_DEFAULT_MODE_INSERT,) -std=c++20 -O0 -Wall -Wextra -Wno-unused-parameter -I./include/ -DPREFIX=\"$(shell realpath $(PREFIX))\"
LDFLAGS := -lstdc++ -lm $(if $(uselibfmt), -lfmt,)

TARGET := ./vidd

CSRCS=$(shell find ./src/ -name "*.cpp" -type f)
OBJS=$(CSRCS:.cpp=.o)

all: $(TARGET)

install: $(TARGET)
	if [ ! -d $(PREFIX)/share/$(TARGET) ]; then \
		mkdir -p $(PREFIX); \
		mkdir -p $(PREFIX)/bin; \
		mkdir -p $(PREFIX)/share; \
		mkdir -p $(PREFIX)/share/$(TARGET); \
		mkdir -p $(PREFIX)/share/$(TARGET)/filedata; \
		chmod -R a+rwx $(PREFIX)/share/$(TARGET); \
	fi
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

