PREFIX=~/.local
RPREFIX=$(shell realpath ${PREFIX})

CC=gcc
CFLAGS=-Wall -g -Wno-pointer-sign -DPREFIX=\"${RPREFIX}\"
CSRCS=$(shell find ./src/ -name '*.c')

all:
	mkdir -p ${PREFIX}/share/vidd/
	mkdir -p ${PREFIX}/share/vidd/filedata/
	touch ${PREFIX}/share/vidd/cpybuf.data
	$(CC) ${CSRCS} -o ./vidd ${CFLAGS}
install: all
	cp ./vidd ${PREFIX}/bin/
tc: all
	./vidd ./test.c
edit:
	vim -p ./src/*.h ./src/*.c