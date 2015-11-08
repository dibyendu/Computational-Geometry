all : libgui.so link

libgui.so : gui.o
	gcc -shared -Wl,-soname,libgui.so.1 -o libgui.so.1.0 gui.o

gui.o :
	gcc -Wall `pkg-config --cflags gtk+-2.0` -std=gnu99 -fPIC -c gui.c

link :
	ln -sf ./libgui.so.1.0 ./libgui.so
	ln -sf ./libgui.so.1.0 ./libgui.so.1

clean :
	rm -f *.o *.so*