RELEASE=0

CC=gcc
INCLUDE=-I./src
INSTALL_DIR=$(HOME)/.local/bin

ifeq ($(RELEASE),0)
    CFLAGS=$(INCLUDE) -Wall -Wextra -pedantic -g -finput-charset=utf-8
else
    CFLAGS=$(INCLUDE) -Wall -Wextra -pedantic -finput-charset=utf-8 -O3
endif

repa: bin/ build/main.o build/hashmap.o
	$(CC) $(CFLAGS) build/hashmap.o build/main.o -o bin/repa

build/:
	mkdir -p build

bin/:
	mkdir -p bin

build/main.o: build/
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/hashmap.o: build/
	$(CC) $(CFLAGS) -c src/hashmap.c -o build/hashmap.o

clean:
	rm -r build
	rm -r bin
	rm demo.gif

run: repa
	./bin/repa

install: repa
	cp ./bin/repa $(INSTALL_DIR)/repa

demo: install
	vhs demo.tape --publish
