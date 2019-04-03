CC = g++
CFLAGS = -W -Wall -Wextra -pedantic -g -I.
SRC = main.o graphics.o game.o

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(SRC)
	$(CC) -o snake main.o graphics.o game.o -lpthread -Llib -lSDL2 -lSDL2main -lSDL2_image

clear:
	rm ./*.o snake
