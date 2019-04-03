CC = gcc
CFLAGS = -W -Wall -Wextra -pedantic -I.
SRC = bot.o main.o mem_manager.o

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(SRC)
	$(CC) -o bot $(SRC)

clear:
	rm ./*.o bot