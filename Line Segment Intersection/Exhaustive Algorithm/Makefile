CC=gcc
CFLAGS=-c -O3 -pipe `pkg-config --cflags gtk+-2.0` -std=gnu99
LDFLAGS=-lm `pkg-config --libs gtk+-2.0`

all: linesegment clean run

linesegment: main.o LineSegmentIntersection.o
	$(CC) -o linesegment main.o LineSegmentIntersection.o $(LDFLAGS)

main.o: main.c
	$(CC) $(CFLAGS) main.c

LineSegmentIntersection.o: LineSegmentIntersection.c
	$(CC) $(CFLAGS) LineSegmentIntersection.c

clean:
	rm *.o

run:
	./linesegment && rm linesegment