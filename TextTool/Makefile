all : text

CFLAGS:=-I.. -I.

MINGW32:=/usr/bin/i686-w64-mingw32-

text.exe : text.c
	$(MINGW32)gcc -m32 -o $@ $^  -lgdi32 $(CFLAGS)


text : text.c
	gcc -o $@ $^ -lX11 -lm -lpthread -lXinerama -lXext -lGL -g $(CFLAGS)

clean :
	rm -rf *.o *~ text

