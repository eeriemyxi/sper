RELEASE=0

CC=gcc
INCLUDE=-I./src
INSTALL_DIR=$(HOME)/.local/bin

ifeq ($(RELEASE),0)
    CFLAGS=$(INCLUDE) -Wall -Wextra -pedantic -g -finput-charset=utf-8
else
    CFLAGS=$(INCLUDE) -static -static-libgcc -Wall -Wextra -pedantic -finput-charset=utf-8 -O3
endif

sper: bin/ build/main.o 
	$(CC) $(CFLAGS) build/main.o -o bin/sper

build/:
	mkdir -p build

bin/:
	mkdir -p bin

build/main.o: build/
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

clean:
	rm -r build
	rm -r bin
	rm demo.gif

run: sper
	./bin/sper

install: sper
	cp ./bin/sper $(INSTALL_DIR)/sper

demo: install
	vhs demo.tape --publish
