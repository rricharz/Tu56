LIBS = `pkg-config --libs gtk+-3.0`

CFLAGS = `pkg-config --cflags gtk+-3.0`

all: tu56 demo

tu56: tu56.c
	gcc -o tu56 tu56.c $(LIBS) $(CFLAGS)

demo: demo.c
	gcc -o demo demo.c
