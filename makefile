all:
	gcc ./src/vidd.c -o ./vidd -lm -lpthread
install:
	sudo cp ./vidd /usr/bin/
tc: all
	./test/test
