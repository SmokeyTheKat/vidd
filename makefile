PREFIX=~/.local

CC=gcc
CFLAGS=-Wall -g -Wno-pointer-sign
CSRCS=$(shell find ./src/ -name '*.c')

all:
	mkdir -p ${PREFIX}/share/vidd/
	mkdir -p ${PREFIX}/share/vidd/filedata/
	touch ${PREFIX}/share/vidd/cpybuf.data
	$(CC) ${CSRCS} -o ./vidd ${CFLAGS}
install: all
	cp ./vidd ${PREFIX}/bin/
tc: all
	./vidd ./test
edit:
	vim -p ./src/*.h ./src/*.c
