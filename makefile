PREFIX=~/.local

CC=gcc
CFLAGS=-Wall -g -Ofast  -Wno-pointer-sign -DPREFIX=\"$(shell realpath ${PREFIX})\" -I./include/
TARGET=./vidd
BUILDDIR=build
SRCDIR=src

CSRCS=$(shell find ./src/ -name '*.c' -printf "%f\n")
OBJS=$(addprefix ${BUILDDIR}/, $(CSRCS:.c=.o))

all: viddpaths ${TARGET}

install: all
	cp ./vidd ${PREFIX}/bin/

${TARGET}: ${BUILDDIR} ${OBJS}
	$(CC) -o ./vidd ${OBJS} ${CFLAGS}

${BUILDDIR}:
	mkdir -p ${BUILDDIR}

${BUILDDIR}/%.o: ${SRCDIR}/%.c
	$(CC) -c ${CFLAGS} -o $@ $<

viddpaths:
	@mkdir -p ${PREFIX}
	@mkdir -p ${PREFIX}/bin
	@mkdir -p ${PREFIX}/share

	@if [ ! -d ${PREFIX}/share/vidd ]; then      \
		mkdir -p ${PREFIX}/share/vidd;             \
		mkdir -p ${PREFIX}/share/vidd/filedata;    \
		touch ${PREFIX}/share/vidd/cpybuf.data;    \
		chmod -R a+rwx ${PREFIX}/share/vidd;       \
	fi

clean:
	rm -rf ${BUILDDIR}
	rm -rf ${TARGET}

tc: all
	./vidd src/main.c

