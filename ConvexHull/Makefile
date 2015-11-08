CC=gcc
CFLAGS=-c -O3 -pipe `pkg-config --cflags gtk+-2.0` -std=gnu99
LDFLAGS=-lm `pkg-config --libs gtk+-2.0`

all: convexhull clean run

convexhull: main.o ConvexHull.o
	$(CC) -o convexhull main.o ConvexHull.o  $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) main.c

ConvexHull.o: ConvexHull.c
	$(CC) $(CFLAGS) ConvexHull.c

clean:
	rm *.o

run:
	./convexhull && rm convexhull
