all:
	gcc ./test/test.c -o ./test/test -lm -lpthread
run: all
	./test/test
