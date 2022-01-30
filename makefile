PREFIX=~/.local
RPREFIX=$(shell realpath ${PREFIX})

CC=gcc
CFLAGS=-Wall -g -Wno-pointer-sign -DPREFIX=\"${RPREFIX}\" -I./include/
CSRCS=$(shell find ./src/ -name '*.c')
OBJS=$(CSRCS:.c=.o)

all: ${OBJS}
	@if [[ ! -d ${PREFIX}/share/vidd ]]; then      \
		mkdir -p ${PREFIX}/share/vidd;            \
		mkdir -p ${PREFIX}/share/vidd/filedata;   \
		touch ${PREFIX}/share/vidd/cpybuf.data;   \
		chmod -R a+rwx ${PREFIX}/share/vidd;      \
	fi
	$(CC) -o ./vidd ${OBJS} ${CFLAGS}

%.o: %.c
	$(CC) -c ${CFLAGS} -o $@ $<

clean:
	find ./ -type f -name '*.o' -delete

install: all
	cp ./vidd ${PREFIX}/bin/

tc: all
	./vidd ./test.c
