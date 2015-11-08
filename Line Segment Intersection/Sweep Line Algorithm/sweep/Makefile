all : sweep draw

sweep :
	javac Sweep.java
	clear
	java Sweep input output

draw :
	gcc -Wall `pkg-config --cflags --libs gtk+-2.0` -L../gui -lgui draw.c -o draw

clean :
	rm -f *.class draw