CC=gcc
CFLAGS=-c -O3 -pipe `pkg-config --cflags gtk+-2.0` -Wall -std=gnu99
LDFLAGS=-lm `pkg-config --libs gtk+-2.0`

all: kd_tree clean run

kd_tree: main.o kd_tree.o
	$(CC) -o kd_tree main.o kd_tree.o  $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) main.c

kd_tree.o: kd_tree.c
	$(CC) $(CFLAGS) kd_tree.c

clean:
	rm *.o

run:
	./kd_tree && rm kd_tree