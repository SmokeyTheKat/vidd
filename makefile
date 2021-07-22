PREFIX=~/.local

CC=gcc
CFLAGS=-Wall -g
CSRCS=$(shell find ./src/ -name '*.c')

all:
	$(CC) ${CSRCS} -o ./vidd ${CFLAGS}
install: all
	cp ./vidd ${PREFIX}/bin/
tc: all
	./vidd ./test
edit:
	vim -p ./src/*.h ./src/*.c
