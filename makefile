all:
	gcc ./src/vidd.c -o ./vidd -Wall -lm -lpthread
install:
	sudo cp ./vidd /usr/bin/
tc: all
	./vidd ./test
