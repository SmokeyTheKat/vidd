all:
	gcc ./src/vidd.c -o ./vidd -lm -lpthread
tc: all
	./test/test
