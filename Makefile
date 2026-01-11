.POSIX:

PREFIX = /usr/local/bin/
CC = clang
INC = /usr/local/include 
LIB = /usr/local/lib 
LIBS = -lX11 -lXft `pkg-config --cflags freetype2`

poke: poke.h
	$(CC) -std=c89 -O2 -pedantic -I$(INC) -L$(LIB) $(LIBS) poke.c -o poke

install: poke
	cp poke $(PREFIX)

uninstall:
	rm $(PREFIX)/poke
